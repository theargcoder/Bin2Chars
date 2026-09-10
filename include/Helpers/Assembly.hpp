#pragma once

#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <sched.h>
#include <utility>
#if defined(__x86_64__)
#include <chrono>
#include <thread>
#include <x86intrin.h>
#endif

namespace Bin2Chars::Helpers::Assembly
{
  inline __attribute__((always_inline)) static auto umulh32(const uint64_t &a, const uint32_t &b) noexcept
  {
    return static_cast<uint32_t>((a * b) >> 32U);
  }

  inline __attribute__((always_inline)) static uint64_t umulh64(const uint64_t &a, const uint64_t &b) noexcept
  {
#if defined(__x86_64__)
    uint64_t hi;
    uint64_t lo = a;
    asm("mul %[b]" : "+a"(lo), "=d"(hi) : [b] "r"(b) : "cc");
    return hi;
#elif defined(__aarch64__)
    uint64_t hi;
    asm("umulh %0, %1, %2" : "=r"(hi) : "r"(a), "r"(b));
    return hi;
#else
    return (uint64_t)((__uint128_t)a * b >> 64U);
#endif
  }

#if defined(__x86_64__)

  inline __attribute__((always_inline)) uint64_t timer_start() noexcept
  {
    asm volatile("" ::: "memory");

    _mm_lfence();
    const uint64_t t = __rdtsc();
    _mm_lfence();

    asm volatile("" ::: "memory");

    return t;
  }

  inline __attribute__((always_inline)) uint64_t timer_end() noexcept
  {
    unsigned int unused;
    const uint64_t t = __rdtscp(&unused);
    _mm_lfence();

    asm volatile("" ::: "memory");

    return t;
  }

#elif defined(__aarch64__)

  inline __attribute__((always_inline)) uint64_t timer_start() noexcept
  {
    asm volatile("isb" ::: "memory");

    uint64_t t;
    asm volatile("mrs %0, cntvct_el0" : "=r"(t) : : "memory");

    return t;
  }

  inline __attribute__((always_inline)) uint64_t timer_end() noexcept
  {
    asm volatile("isb" ::: "memory");

    uint64_t t;
    asm volatile("mrs %0, cntvct_el0" : "=r"(t) : : "memory");

    asm volatile("isb" ::: "memory");

    return t;
  }

#endif

#if defined(__x86_64__)

  inline uint64_t rdtsc_freq()
  {
    // This is the tricky part on x86
    // Best option: calibrate once using chrono
    static uint64_t freq = []
    {
      using namespace std::chrono;

      auto start_tsc = Helpers::Assembly::timer_start();
      auto start = steady_clock::now();

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      auto end_tsc = Helpers::Assembly::timer_end();
      auto end = steady_clock::now();

      auto ns = static_cast<uint64_t>(duration_cast<nanoseconds>(end - start).count());
      return (end_tsc - start_tsc) * 1'000'000'000ULL / ns;
    }();

    return freq;
  }

#elif defined(__aarch64__)

  inline uint64_t rdtsc_freq()
  {
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
  }

#else
#error "Unsupported architecture"
#endif

  inline uint64_t rdtsc_to_ns(uint64_t ticks)
  {
    static const uint64_t freq = rdtsc_freq();

    // avoid overflow with 128-bit math
    return static_cast<uint64_t>(static_cast<__uint128_t>(ticks) * 1'000'000'000ULL / freq);
  }

  inline void pin_thread_to_cpu(const unsigned &cpu_id)
  {
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
    cpu_set_t allowed;
    CPU_ZERO(&allowed); // initialize

    if(sched_getaffinity(0, sizeof(allowed), &allowed) != 0)
    {
      perror("sched_getaffinity");
      std::terminate();
    }

    if(!CPU_ISSET(cpu_id, &allowed))
    {
      std::cerr << "CPU " << cpu_id << " not allowed in this cgroup\n";
      std::terminate();
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    if(sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0)
    {
      perror("sched_setaffinity");
      std::terminate();
    }
#elif defined(__ARM_NEON) || defined(__aarch64__)
#endif
  }

  template <std::size_t N, typename T>
  void prefetch_elements(const T *ptr)
  {
    const char *base_addr = reinterpret_cast<const char *>(ptr + 1);

    constexpr std::size_t total_bytes = N * sizeof(T);

    constexpr std::size_t num_lines = (total_bytes + 63) / 64 + 1;

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
    [&]<std::size_t... Is>(std::index_sequence<Is...>) { (..., _mm_prefetch(base_addr + (Is * 64), _MM_HINT_T0)); }(std::make_index_sequence<num_lines>{});

#elif defined(__ARM_NEON) || defined(__aarch64__)
    [&]<std::size_t... Is>(std::index_sequence<Is...>) { (..., __builtin_prefetch(base_addr + (Is * 64), 0, 3)); }(std::make_index_sequence<num_lines>{});
#endif
  }

} // namespace Bin2Chars::Helpers::Assembly
