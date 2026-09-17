#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <thread>
#include <utility>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <intrin.h>
#include <windows.h>
#if defined(_M_X64) || defined(_M_IX86)
#include <immintrin.h>
#endif
#elif defined(__x86_64__) || defined(__i386__)
#include <sched.h>
#include <x86intrin.h>
#elif defined(__aarch64__)
// No architecture-specific header needed for GNU/Clang inline assembly.
#endif

#if defined(_MSC_VER)
#define BIN2CHARS_ALWAYS_INLINE __forceinline
#define BIN2CHARS_COMPILER_BARRIER() _ReadWriteBarrier()
#elif defined(__GNUC__) || defined(__clang__)
#define BIN2CHARS_ALWAYS_INLINE inline __attribute__((always_inline))
#define BIN2CHARS_COMPILER_BARRIER() asm volatile("" ::: "memory")
#else
#define BIN2CHARS_ALWAYS_INLINE inline
#define BIN2CHARS_COMPILER_BARRIER() ((void)0)
#endif

namespace Bin2Chars::Helpers::Assembly
{
  BIN2CHARS_ALWAYS_INLINE uint32_t umulh32(const uint64_t &a, const uint32_t &b) noexcept
  {
    // Preserves the original operation: multiply modulo 2^64, then take bits 32..63.
    return static_cast<uint32_t>((a * b) >> 32U);
  }

  BIN2CHARS_ALWAYS_INLINE uint64_t umulh64(const uint64_t &a, const uint64_t &b) noexcept
  {
#if defined(_MSC_VER) && defined(_M_X64)
    uint64_t hi;
    (void)_umul128(a, b, &hi); // Return value is low half; pointer receives high half.
    return hi;

#elif defined(__x86_64__)
    uint64_t hi;
    uint64_t lo = a;
    asm("mul %[b]" : "+a"(lo), "=d"(hi) : [b] "r"(b) : "cc");
    return hi;

#elif defined(__aarch64__) && !defined(_MSC_VER)
    uint64_t hi;
    asm("umulh %0, %1, %2" : "=r"(hi) : "r"(a), "r"(b));
    return hi;

#else
    // Portable high-half multiplication without requiring __uint128_t.
    const uint64_t a_lo = static_cast<uint32_t>(a);
    const uint64_t a_hi = a >> 32U;
    const uint64_t b_lo = static_cast<uint32_t>(b);
    const uint64_t b_hi = b >> 32U;

    const uint64_t p00 = a_lo * b_lo;
    const uint64_t p01 = a_lo * b_hi;
    const uint64_t p10 = a_hi * b_lo;
    const uint64_t p11 = a_hi * b_hi;

    const uint64_t carry = (p00 >> 32U) + static_cast<uint32_t>(p01) + static_cast<uint32_t>(p10);

    return p11 + (p01 >> 32U) + (p10 >> 32U) + (carry >> 32U);
#endif
  }

#if defined(_M_X64) || defined(__x86_64__)

  BIN2CHARS_ALWAYS_INLINE uint64_t timer_start() noexcept
  {
    BIN2CHARS_COMPILER_BARRIER();

    _mm_lfence();
    const uint64_t t = __rdtsc();
    _mm_lfence();

    BIN2CHARS_COMPILER_BARRIER();
    return t;
  }

  BIN2CHARS_ALWAYS_INLINE uint64_t timer_end() noexcept
  {
    unsigned int unused = 0;
    const uint64_t t = __rdtscp(&unused);
    _mm_lfence();

    BIN2CHARS_COMPILER_BARRIER();
    return t;
  }

#elif defined(_M_ARM64)

  // Windows ARM64: use the OS performance counter.
  BIN2CHARS_ALWAYS_INLINE uint64_t timer_start() noexcept
  {
    LARGE_INTEGER value{};
    QueryPerformanceCounter(&value);
    return static_cast<uint64_t>(value.QuadPart);
  }

  BIN2CHARS_ALWAYS_INLINE uint64_t timer_end() noexcept
  {
    LARGE_INTEGER value{};
    QueryPerformanceCounter(&value);
    return static_cast<uint64_t>(value.QuadPart);
  }

#elif defined(__aarch64__)

  BIN2CHARS_ALWAYS_INLINE uint64_t timer_start() noexcept
  {
    asm volatile("isb" ::: "memory");

    uint64_t t;
    asm volatile("mrs %0, cntvct_el0" : "=r"(t) : : "memory");
    return t;
  }

  BIN2CHARS_ALWAYS_INLINE uint64_t timer_end() noexcept
  {
    asm volatile("isb" ::: "memory");

    uint64_t t;
    asm volatile("mrs %0, cntvct_el0" : "=r"(t) : : "memory");

    asm volatile("isb" ::: "memory");
    return t;
  }

#else
#error "Unsupported architecture for timer functions"
#endif

#if defined(_M_X64) || defined(__x86_64__)

  inline uint64_t rdtsc_freq()
  {
    static const uint64_t freq = []
    {
      using namespace std::chrono;

      const auto start_tsc = timer_start();
      const auto start = steady_clock::now();

      std::this_thread::sleep_for(milliseconds(100));

      const auto end_tsc = timer_end();
      const auto end = steady_clock::now();

      const auto ns = static_cast<uint64_t>(duration_cast<nanoseconds>(end - start).count());

      return ns == 0 ? uint64_t{ 0 } : (end_tsc - start_tsc) * 1'000'000'000ULL / ns;
    }();

    return freq;
  }

#elif defined(_M_ARM64)

  inline uint64_t rdtsc_freq()
  {
    LARGE_INTEGER frequency{};
    if(!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0)
      return 0;

    return static_cast<uint64_t>(frequency.QuadPart);
  }

#elif defined(__aarch64__)

  inline uint64_t rdtsc_freq()
  {
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
  }

#else
#error "Unsupported architecture for rdtsc_freq"
#endif

  inline uint64_t rdtsc_to_ns(uint64_t ticks)
  {
    static const uint64_t freq = rdtsc_freq();

    if(freq == 0)
      return 0;

#if defined(_MSC_VER) && defined(_M_X64)
    // Exact 128-bit product and division using MSVC x64 intrinsics.
    uint64_t high = 0;
    const uint64_t low = _umul128(ticks, 1'000'000'000ULL, &high);

    // _udiv128 requires the quotient to fit in 64 bits.
    if(high >= freq)
      return static_cast<uint64_t>((static_cast<long double>(ticks) * 1'000'000'000.0L) / static_cast<long double>(freq));

    uint64_t remainder = 0;
    return _udiv128(high, low, freq, &remainder);

#elif defined(__SIZEOF_INT128__)
    return static_cast<uint64_t>((static_cast<__uint128_t>(ticks) * 1'000'000'000ULL) / freq);

#else
    // Fallback for compilers without a native 128-bit integer type.
    return static_cast<uint64_t>((static_cast<long double>(ticks) * 1'000'000'000.0L) / static_cast<long double>(freq));
#endif
  }

#if defined(_WIN32) && (defined(_M_X64) || defined(_M_IX86))

  inline void pin_thread_to_cpu(const unsigned &cpu_id)
  {
    constexpr unsigned processor_count = static_cast<unsigned>(sizeof(DWORD_PTR) * 8U);

    if(cpu_id >= processor_count)
    {
      std::cerr << "CPU " << cpu_id << " cannot be selected with SetThreadAffinityMask\n";
      std::terminate();
    }

    const DWORD_PTR mask = DWORD_PTR{ 1 } << cpu_id;

    if(SetThreadAffinityMask(GetCurrentThread(), mask) == 0)
    {
      std::cerr << "SetThreadAffinityMask failed: " << GetLastError() << '\n';
      std::terminate();
    }
  }

#elif defined(__linux__) && (defined(__x86_64__) || defined(__i386__))

  inline void pin_thread_to_cpu(const unsigned &cpu_id)
  {
    cpu_set_t allowed;
    CPU_ZERO(&allowed);

    if(sched_getaffinity(0, sizeof(allowed), &allowed) != 0)
    {
      std::perror("sched_getaffinity");
      std::terminate();
    }

    if(cpu_id >= CPU_SETSIZE || !CPU_ISSET(cpu_id, &allowed))
    {
      std::cerr << "CPU " << cpu_id << " not allowed in this process/cgroup\n";
      std::terminate();
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    if(sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0)
    {
      std::perror("sched_setaffinity");
      std::terminate();
    }
  }

#elif defined(__ARM_NEON) || defined(__aarch64__)

  // Preserves the original ARM behavior: no affinity change is performed.
  inline void pin_thread_to_cpu(const unsigned &cpu_id)
  {
#if defined(_MSC_VER)
    (void)cpu_id;
#else
    asm volatile("" : : "m"(cpu_id), "r"(cpu_id) : "memory");
#endif
  }

#endif

  template <std::size_t N, typename T>
  void prefetch_elements(const T *ptr)
  {
    const char *base_addr = reinterpret_cast<const char *>(ptr);

    constexpr std::size_t total_bytes = N * sizeof(T);
    constexpr std::size_t num_lines = (total_bytes + 63U) / 64U + 1U;

#if defined(_M_X64) || defined(_M_IX86) || defined(__x86_64__) || defined(__i386__)

    [&]<std::size_t... Is>(std::index_sequence<Is...>) { (..., _mm_prefetch(base_addr + (Is * 64U), _MM_HINT_T0)); }(std::make_index_sequence<num_lines>{});

#elif (defined(__ARM_NEON) || defined(__aarch64__)) && (defined(__GNUC__) || defined(__clang__))

    [&]<std::size_t... Is>(std::index_sequence<Is...>) { (..., __builtin_prefetch(base_addr + (Is * 64U), 0, 3)); }(std::make_index_sequence<num_lines>{});

#else
    (void)base_addr;
#endif
  }

} // namespace Bin2Chars::Helpers::Assembly

#undef BIN2CHARS_ALWAYS_INLINE
#undef BIN2CHARS_COMPILER_BARRIER
