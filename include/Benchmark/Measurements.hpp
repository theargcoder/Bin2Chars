#pragma once

#include <linux/perf_event.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <x86intrin.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

namespace Bin2Chars::Benchmark::Measurements
{
  struct CounterSnapshot
  {
    std::uint64_t tsc_ticks;
    std::uint64_t clk_cycles;
    std::uint64_t ref_cycles;
  };

  class PmuTimer
  {
  public:
    PmuTimer(const PmuTimer &) = delete;
    PmuTimer &operator=(const PmuTimer &) = delete;
    PmuTimer(PmuTimer &&) = delete;
    PmuTimer &operator=(PmuTimer &&) = delete;

  private:
    int core_fd_ = -1;
    int ref_fd_ = -1;

    perf_event_mmap_page *core_page_ = nullptr;
    perf_event_mmap_page *ref_page_ = nullptr;

    std::size_t page_size_ = 0;

  private:
    static int open_event(const std::uint64_t config, const int group_fd)
    {
      perf_event_attr attr{};

      attr.size = sizeof(attr);
      attr.type = PERF_TYPE_HARDWARE;
      attr.config = config;

      const bool leader = group_fd == -1;

      // Only the group leader is pinned and initially disabled.
      attr.pinned = leader;
      attr.disabled = leader;

      // Only count user-space execution.
      attr.exclude_kernel = 1;
      attr.exclude_hv = 1;

      const long fd = syscall(SYS_perf_event_open, &attr,
                              0,  // current thread
                              -1, // CPU on which the thread runs
                              group_fd, PERF_FLAG_FD_CLOEXEC);

      if(fd == -1)
      {
        throw std::runtime_error("perf_event_open(" + std::to_string(config) + ", group_fd=" + std::to_string(group_fd) + "): " + std::strerror(errno));
      }

      return static_cast<int>(fd);
    }

    static perf_event_mmap_page *map_page(const int fd, const std::size_t page_size)
    {
      void *ptr = ::mmap(nullptr, page_size, PROT_READ, MAP_SHARED, fd, 0);

      if(ptr == MAP_FAILED)
      {
        throw std::runtime_error("mmap: " + std::string(std::strerror(errno)));
      }

      return static_cast<perf_event_mmap_page *>(ptr);
    }

    static std::uint64_t rdpmc(const std::uint32_t ecx) noexcept
    {
      std::uint32_t lo;
      std::uint32_t hi;

      asm volatile("rdpmc" : "=a"(lo), "=d"(hi) : "c"(ecx) : "memory");

      return (static_cast<std::uint64_t>(hi) << 32U) | static_cast<std::uint64_t>(lo);
    }

    static std::uint64_t read_counter(const perf_event_mmap_page *page) noexcept
    {
      for(;;)
      {
        const std::uint32_t seq = __atomic_load_n(&page->lock, __ATOMIC_ACQUIRE);

        const std::uint32_t index = page->index;
        const std::uint16_t width = page->pmc_width;
        const std::int64_t offset = page->offset;

        // Event temporarily not running.
        if(index == 0)
        {
          continue;
        }

        std::uint64_t raw = rdpmc(index - 1U);

        // Sign-extend the PMU counter width.
        if(width < 64U)
        {
          const std::uint64_t sign_bit = std::uint64_t{ 1 } << (width - 1U);

          const std::uint64_t mask = (std::uint64_t{ 1 } << width) - 1U;

          raw &= mask;

          if((raw & sign_bit) != 0U)
          {
            raw |= ~mask;
          }
        }

        const auto pmc = static_cast<std::int64_t>(raw);

        const auto value = static_cast<std::uint64_t>(pmc + offset);

        __atomic_thread_fence(__ATOMIC_ACQUIRE);

        if(__atomic_load_n(&page->lock, __ATOMIC_RELAXED) == seq)
        {
          return value;
        }
      }
    }

  public:
    PmuTimer()
    {
      const long raw_page_size = sysconf(_SC_PAGESIZE);

      if(raw_page_size <= 0)
      {
        throw std::runtime_error("sysconf(_SC_PAGESIZE) failed");
      }

      page_size_ = static_cast<std::size_t>(raw_page_size);

      // Group leader: actual core cycles.
      core_fd_ = open_event(PERF_COUNT_HW_CPU_CYCLES, -1);

      try
      {
        // Group member: reference cycles.
        ref_fd_ = open_event(PERF_COUNT_HW_REF_CPU_CYCLES, core_fd_);

        core_page_ = map_page(core_fd_, page_size_);

        ref_page_ = map_page(ref_fd_, page_size_);

        // cap_user_rdpmc is capability bit 2.
        constexpr std::uint64_t CAP_USER_RDPMC = 1ULL << 2U;

        if((core_page_->capabilities & CAP_USER_RDPMC) == 0U || (ref_page_->capabilities & CAP_USER_RDPMC) == 0U)
        {
          throw std::runtime_error("RDPMC is not available");
        }

        // Reset both counters together.
        if(ioctl(core_fd_, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP) == -1)
        {
          throw std::runtime_error("PERF_EVENT_IOC_RESET: " + std::string(std::strerror(errno)));
        }

        // Start both counters together.
        if(ioctl(core_fd_, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP) == -1)
        {
          throw std::runtime_error("PERF_EVENT_IOC_ENABLE: " + std::string(std::strerror(errno)));
        }
      }
      catch(...)
      {
        if(ref_page_ != nullptr)
        {
          munmap(ref_page_, page_size_);

          ref_page_ = nullptr;
        }

        if(core_page_ != nullptr)
        {
          munmap(core_page_, page_size_);

          core_page_ = nullptr;
        }

        if(ref_fd_ != -1)
        {
          close(ref_fd_);
          ref_fd_ = -1;
        }

        if(core_fd_ != -1)
        {
          close(core_fd_);
          core_fd_ = -1;
        }

        throw;
      }
    }

    ~PmuTimer()
    {
      if(core_fd_ != -1)
      {
        (void)ioctl(core_fd_, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
      }

      if(ref_page_ != nullptr)
      {
        munmap(ref_page_, page_size_);
      }

      if(core_page_ != nullptr)
      {
        munmap(core_page_, page_size_);
      }

      if(ref_fd_ != -1)
      {
        close(ref_fd_);
      }

      if(core_fd_ != -1)
      {
        close(core_fd_);
      }
    }

    [[nodiscard]] CounterSnapshot start() const noexcept
    {
      _mm_lfence();

      const std::uint64_t tsc = __rdtsc();
      const std::uint64_t core = read_counter(core_page_);
      const std::uint64_t ref = read_counter(ref_page_);

      _mm_lfence();

      return { .tsc_ticks = tsc, .clk_cycles = core, .ref_cycles = ref };
    }

    [[nodiscard]] CounterSnapshot stop() const noexcept
    {
      _mm_lfence();

      const std::uint64_t core = read_counter(core_page_);
      const std::uint64_t ref = read_counter(ref_page_);
      const std::uint64_t tsc = __rdtsc();

      _mm_lfence();

      return { .tsc_ticks = tsc, .clk_cycles = core, .ref_cycles = ref };
    }

    [[nodiscard]] static CounterSnapshot delta(const CounterSnapshot &begin, const CounterSnapshot &end) noexcept
    {
      return { .tsc_ticks = end.tsc_ticks - begin.tsc_ticks, .clk_cycles = end.clk_cycles - begin.clk_cycles, .ref_cycles = end.ref_cycles - begin.ref_cycles };
    }
  };

} // namespace Bin2Chars::Benchmark::Measurements
