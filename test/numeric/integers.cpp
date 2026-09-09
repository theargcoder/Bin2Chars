#define BOOST_TEST_MODULE IntegersTest
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "include/Algos/Competition.hpp"
#include "include/Algos/Integer.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Tests.hpp"

using namespace Bin2Chars::Tests;

namespace
{
  template <uint64_t N, typename Type>
  auto looper_ints(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &bin2chars_time, auto &bin2chars_cpu_cycles, auto &std_lib_time, auto &std_lib_cpu_cycles,
                   auto &std_lib_to_str_time, auto &std_lib_to_str_cycles, auto &simdy_time, auto &simdy_cycles) -> void
  {
    const constexpr auto WISHED_RANGE = 100'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    uint32_t errors = 0;
    uint64_t cycles = 0;

    std::string our_log, std_log, std_lib_to_str_log, simdy_log;
    while(cycles < WISHED_RANGE && errors < MAX_ERRORS)
    {
      cycles += RANGE;
      using LoopType = std::intmax_t;

      const auto delim = static_cast<LoopType>(DELIM);
      const auto jump = static_cast<LoopType>(JUMP);
      const auto range = static_cast<LoopType>(RANGE);

      for(LoopType i = delim, lim = 0, max_iter = 0; (PLUS ? i < delim + range : i > delim - range) && lim < static_cast<LoopType>(MAX_ERRORS) && max_iter < range;
          (PLUS ? i += jump : i -= jump), ++max_iter)
      {
        const auto st_log = Bin2Chars::Helpers::Assembly::timer_start();
        our_log = Bin2Chars::Numeric::Integral::ToStr(i);
        const auto en_log = Bin2Chars::Helpers::Assembly::timer_end();

        const auto st_std_to_str = Bin2Chars::Helpers::Assembly::timer_start();
        std_log = Bin2Chars::Numeric::Std::to_string<false>(i, 123);
        const auto en_std_to_str = Bin2Chars::Helpers::Assembly::timer_end();

        const auto std_lib_to_st = Bin2Chars::Helpers::Assembly::timer_start();
        std_lib_to_str_log = std::to_string(i);
        const auto std_lib_to_en = Bin2Chars::Helpers::Assembly::timer_end();

        const auto simdy_st = Bin2Chars::Helpers::Assembly::timer_start();
        simdy_log = Bin2Chars::Numeric::Integral::ToStrSIMD(i);
        const auto simdy_en = Bin2Chars::Helpers::Assembly::timer_end();

        bin2chars_time += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_log - st_log)));
        bin2chars_cpu_cycles += en_log - st_log;
        std_lib_time += std::chrono::duration_cast<std::chrono::nanoseconds>(
            static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_std_to_str - st_std_to_str)));
        std_lib_cpu_cycles += en_std_to_str - st_std_to_str;
        std_lib_to_str_time += std::chrono::duration_cast<std::chrono::nanoseconds>(
            static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(std_lib_to_en - std_lib_to_st)));
        std_lib_to_str_cycles += std_lib_to_en - std_lib_to_st;
        simdy_time += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(simdy_en - simdy_st)));
        simdy_cycles += simdy_en - simdy_st;

        if(our_log != std_log || simdy_log != std_lib_to_str_log)
        {
          BOOST_CHECK_EQUAL(our_log, std_log);
          log_str_and_into_hex(LogHexStr("Bin2Chars::Numeric::ToStr", our_log), LogHexStr("std::to_chars", std_log), LogHexStr("std::to_string", std_lib_to_str_log),
                               LogHexStr("Bin2Chars::Numeric::ToStrSIMD", simdy_log));

          lim++;
          errors++;
        }
      }
    }
  }

  template <uint64_t N, typename T>
  auto tester_ints(const T & /*unused*/) -> auto
  {
    std::chrono::nanoseconds tostr_integral_ours_took{ 0 };
    std::chrono::nanoseconds std_to_chars_took{ 0 };
    std::chrono::nanoseconds std_lib_to_str_time{ 0 };
    std::chrono::nanoseconds simdy_lib_time{ 0 };
    uint64_t helpers_math_cpu_cycles{ 0 };
    uint64_t std_lib_cpu_cycles{ 0 };
    uint64_t std_lib_to_str_cycles{ 0 };
    uint64_t simdy_lib_cycles{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_ints<N>(true, MIN, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);
    looper_ints<N>(false, MAX, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);
    ;
    looper_ints<N>(true, T{ 0 }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);
    looper_ints<N>(false, T{ 0 }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = static_cast<T>(UNIT << e);
      looper_ints<N>(true, val, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
      looper_ints<N>(false, val, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
    }

    for(std::intmax_t val = 10; val > 0 && val < static_cast<std::intmax_t>(MAX) / 10; val *= 10)
    {
      looper_ints<N>(true, static_cast<T>(val), UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time,
                     std_lib_to_str_cycles, simdy_lib_time, simdy_lib_cycles);

      looper_ints<N>(false, static_cast<T>(val), UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time,
                     std_lib_to_str_cycles, simdy_lib_time, simdy_lib_cycles);
    }
    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_ints<N>(true, MIN / 2, T{ 123 }, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
      looper_ints<N>(false, MAX / 2, T{ 123 }, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
    }

    // ---- Randomish coverage ----
    looper_ints<N>(true, T{ MAX / T{ 10 } }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time,
                   std_lib_to_str_cycles, simdy_lib_time, simdy_lib_cycles);
    looper_ints<N>(false, T{ MAX / T{ 10 } * T{ 9 } }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time,
                   std_lib_to_str_cycles, simdy_lib_time, simdy_lib_cycles);

    return std::make_tuple(tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles, simdy_lib_time,
                           simdy_lib_cycles);
  }

  template <typename T, size_t... I>
    requires std::is_integral_v<T>
  auto test_and_benchmark_int_impl(std::index_sequence<I...> /*unused*/)
  {
    auto res = tester_ints<1>(T{ 0 });
    ((res = tester_ints<I + 1>(T{ 0 }),
      log_time_tables(T{ 0 }, "INTEGERS", I + 1, BenchResult("Numeric:ToStr", std::get<0>(res), std::get<1>(res)), BenchResult("std::to_chars", std::get<2>(res), std::get<3>(res)),
                      BenchResult("std::to_string", std::get<4>(res), std::get<5>(res)), BenchResult("Numeric::ToStrSIMD", std::get<6>(res), std::get<7>(res)))),
     ...);
  }

  template <typename T>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_ints(T)
  {
    test_and_benchmark_int_impl<T>(std::make_index_sequence<2>{});
  }

} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  Bin2Chars::Helpers::Assembly::pin_thread_to_cpu(3);

  test_and_benchmark_ints<int8_t>(0);
  test_and_benchmark_ints<uint8_t>(0);
  test_and_benchmark_ints<int16_t>(0);
  test_and_benchmark_ints<uint16_t>(0);
  test_and_benchmark_ints<int32_t>(0);
  test_and_benchmark_ints<uint32_t>(0);
  test_and_benchmark_ints<int64_t>(0);
  test_and_benchmark_ints<uint64_t>(0);
}
