#include "include/Algos/Competition.h"
#include "include/Helpers/Math.h"
#include "include/Helpers/Simd.h"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <stdio.h>
#include <vector>

/*
int main(int argc, char **argv)
{
  // 1. Pin to a specific core to avoid cross-core TSC sync issues
  Helpers::Assembly::pin_thread_to_cpu(3);

  static constexpr auto TRIALS = 100'000'000;
  std::vector<uint32_t> random_inputs(TRIALS);
  std::vector<uint64_t> simdy_times(TRIALS);
  std::vector<uint64_t> std_times(TRIALS);

  // 2. Pre-generate randoms to completely destroy std::to_string branch prediction
  std::mt19937 rng(42);
  std::uniform_int_distribution<uint16_t> dist(100, 4294967295); // Mix of digits
  for(int i = 0; i < TRIALS; ++i)
  {
    random_inputs[i] = dist(rng);
  }

  char buff[32];

  // 3. The Measurement Loop
  for(int i = 0; i < TRIALS; ++i)
  {
    uint16_t current_num = random_inputs[i];

    uint64_t st_simdy = Helpers::Assembly::timer_start();

    const auto len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<uint16_t>(&buff[0], current_num);

    uint64_t en_simdy = Helpers::Assembly::timer_end();

    // Force compiler to materialize the result
    asm volatile("" : : "m"(*(char (*)[32])buff), "r"(len) : "memory");

    simdy_times[i] = en_simdy - st_simdy;

    uint64_t st_std = Helpers::Assembly::timer_start();

    const auto pp = std::to_chars(&buff[0], &buff[32], current_num);

    uint64_t en_std = Helpers::Assembly::timer_end();

    // Force compiler to materialize the result
    asm volatile("" : : "m"(*(char (*)[32])buff), "r"(pp) : "memory");
    std_times[i] = en_std - st_std;
  }

  // 4. Statistical Analysis
  std::sort(simdy_times.begin(), simdy_times.end());
  std::sort(std_times.begin(), std_times.end());

  const auto simdy_accum = std::accumulate(simdy_times.begin(), simdy_times.end(), 0ULL);
  const auto std_accum = std::accumulate(std_times.begin(), std_times.end(), 0ULL);

  // The Minimum is the "perfect" hardware run.
  // The Median is the true realistic "Cold" run, ignoring OS interrupts.
  printf("\n=== PERFECT STATS (Cold Data, Unpredictable Branches, %d runs) ===\n", TRIALS);
  printf("SIMDY     | Min: %4lu | Median: %4lu | Mean: %.3f | 95th Percentile: %4lu\n", simdy_times[0], simdy_times[TRIALS / 2],
         static_cast<double>(simdy_accum) / simdy_times.size(), simdy_times[TRIALS * 95 / 100]);
  printf("TO_STRING | Min: %4lu | Median: %4lu | Mean: %.3f | 95th Percentile: %4lu\n", std_times[0], std_times[TRIALS / 2], static_cast<double>(std_accum) / std_times.size(),
         std_times[TRIALS * 95 / 100]);

  return 0;
}
*/

int main(int argc, char **argv)
{
  // 1. Pin to a specific core to avoid cross-core TSC sync issues
  Helpers::Assembly::pin_thread_to_cpu(3);

  static constexpr auto TRIALS = 100'000'000;
  std::vector<uint32_t> random_inputs(TRIALS);
  std::vector<uint64_t> simdy_times(TRIALS);
  std::vector<uint64_t> std_times(TRIALS);

  // 2. Pre-generate randoms to completely destroy std::to_string branch prediction
  std::mt19937 rng(42);
  std::uniform_int_distribution<uint16_t> dist(100, 4294967295); // Mix of digits
  for(int i = 0; i < TRIALS; ++i)
  {
    random_inputs[i] = dist(rng);
  }

  char buff[32];

  // 3. The Measurement Loop
  for(int i = 0; i < TRIALS; ++i)
  {
    uint16_t current_num = random_inputs[i];

    const uint64_t st_simdy = Helpers::Assembly::timer_start();

    const auto len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<uint16_t>(&buff[0], current_num);

    const uint64_t en_simdy = Helpers::Assembly::timer_end();

    // Force compiler to materialize the result
    asm volatile("" : : "m"(*(char (*)[32])buff), "r"(len) : "memory");

    simdy_times[i] = en_simdy - st_simdy;

    const uint64_t st_std = Helpers::Assembly::timer_start();

    const auto pp = Helpers::Numeric::Std::to_chars_impl(&buff[0], current_num);

    const uint64_t en_std = Helpers::Assembly::timer_end();

    // Force compiler to materialize the result
    asm volatile("" : : "m"(*(char (*)[32])buff), "r"(pp) : "memory");
    std_times[i] = en_std - st_std;
  }

  // 4. Statistical Analysis
  std::sort(simdy_times.begin(), simdy_times.end());
  std::sort(std_times.begin(), std_times.end());

  const auto simdy_accum = std::accumulate(simdy_times.begin(), simdy_times.end(), 0ULL);
  const auto std_accum = std::accumulate(std_times.begin(), std_times.end(), 0ULL);

  // The Minimum is the "perfect" hardware run.
  // The Median is the true realistic "Cold" run, ignoring OS interrupts.
  printf("\n=== PERFECT STATS (Cold Data, Unpredictable Branches, %d runs) ===\n", TRIALS);
  printf("SIMDY     | Min: %4lu | Median: %4lu | Mean: %.3f | 95th Percentile: %4lu\n", simdy_times[0], simdy_times[TRIALS / 2],
         static_cast<double>(simdy_accum) / simdy_times.size(), simdy_times[TRIALS * 95 / 100]);
  printf("TO_STRING | Min: %4lu | Median: %4lu | Mean: %.3f | 95th Percentile: %4lu\n", std_times[0], std_times[TRIALS / 2], static_cast<double>(std_accum) / std_times.size(),
         std_times[TRIALS * 95 / 100]);

  return 0;
}
