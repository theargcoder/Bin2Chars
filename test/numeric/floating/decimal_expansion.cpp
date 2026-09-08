#define BOOST_TEST_MODULE DecimalExpansionTest
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <bit>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>

#include "ryu/ryu.h"

#include "include/Algos/Competition.hpp"
#include "include/Algos/Floating/DecimalExpansion.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Tests.hpp"

using namespace Bin2Chars::Tests;

namespace
{
  const auto fuzzer_format_exponential = []<typename Type>(const Type &, const int &PRECISION, const size_t SAMPLES, auto &open_logging_took, auto &open_logging_cycles,
                                                           auto &std_fmt_took, auto &std_cycles, auto &ryu_took, auto &ryu_cycles) -> void
  {
    using UIntType = std::conditional_t<sizeof(Type) == 4, uint32_t, uint64_t>;

    // Fixed seed so test failures are 100% reproducible
    std::mt19937_64 rng(0xDEADBEEF);
    std::uniform_int_distribution<UIntType> dist(0, std::numeric_limits<UIntType>::max());

    size_t errors = 0;
    const constexpr size_t MAX_ERRORS = 10;

    for(size_t i = 0; i < SAMPLES && errors < MAX_ERRORS; ++i)
    {
      UIntType raw_bits = dist(rng);
      Type val = std::bit_cast<Type>(raw_bits);

      std::string open_logging, std_format, ryu;

      const auto st_open_logging = Bin2Chars::Helpers::Assembly::timer_start();
      open_logging = Bin2Chars::Numeric::Floating::DigitsPrecision::ToStr<Bin2Chars::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);
      const auto en_open_logging = Bin2Chars::Helpers::Assembly::timer_end();

      const auto st_std_fmt = Bin2Chars::Helpers::Assembly::timer_start();
      std_format = Bin2Chars::Numeric::Std::to_string<false>(val, PRECISION);
      const auto en_std_fmt = Bin2Chars::Helpers::Assembly::timer_end();

      const auto st_ryu = Bin2Chars::Helpers::Assembly::timer_start();
      ryu = Bin2Chars::Numeric::Ryu::Fixed::ToStr(val, PRECISION);
      const auto en_ryu = Bin2Chars::Helpers::Assembly::timer_end();

      open_logging_took += std::chrono::duration_cast<std::chrono::nanoseconds>(
          static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_open_logging - st_open_logging)));
      open_logging_cycles += en_open_logging - st_open_logging;
      std_fmt_took
          += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_std_fmt - st_std_fmt)));
      std_cycles += en_std_fmt - st_std_fmt;
      ryu_took += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_ryu - st_ryu)));
      ryu_cycles += en_ryu - st_ryu;

      if(open_logging != std_format)
      {
        // for some reason std::to_chars likes to put a '-' before a nan; the IEEE754 standard doesnt define that so idk
        if(open_logging.contains("nan") && std_format.contains("nan"))
        {
          continue;
        }

        BOOST_CHECK_EQUAL(open_logging, std_format);
        log_str_and_into_hex(LogHexStr("open_logging", open_logging), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

        open_logging = Bin2Chars::Numeric::Floating::DigitsPrecision::ToStr<Bin2Chars::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);

        char buffer[1024];
        d2exp_buffered(static_cast<double>(val), static_cast<uint32_t>(PRECISION), &buffer[0]);

        errors++;
      }
    }
  };

  const auto lopper_format_exponential = []<typename Type>(const int &PRECISION, const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging_took,
                                                           auto &open_logging_cycles, auto &std_fmt_took, auto &std_cycles, auto &ryu_took, auto &ryu_cycles) -> void
  {
    const constexpr auto WISHED_RANGE = 100'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? WISHED_RANGE : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    // OpenLogging logger;

    for(Type val = DELIM, lim = 0, max_iter = 0; ((PLUS) ? val < DELIM + RANGE : val > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE;
        (PLUS) ? val += JUMP : val -= JUMP, max_iter++)
    {
      std::string open_logging, std_format, ryu;

      const auto st_open_logging = Bin2Chars::Helpers::Assembly::timer_start();
      open_logging = Bin2Chars::Numeric::Floating::DigitsPrecision::ToStr<Bin2Chars::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);
      const auto en_open_logging = Bin2Chars::Helpers::Assembly::timer_end();

      const auto st_std_fmt = Bin2Chars::Helpers::Assembly::timer_start();
      std_format = Bin2Chars::Numeric::Std::to_string<false>(val, PRECISION);
      const auto en_std_fmt = Bin2Chars::Helpers::Assembly::timer_end();

      const auto st_ryu = Bin2Chars::Helpers::Assembly::timer_start();
      ryu = Bin2Chars::Numeric::Ryu::Fixed::ToStr(val, PRECISION);
      const auto en_ryu = Bin2Chars::Helpers::Assembly::timer_end();

      open_logging_took += std::chrono::duration_cast<std::chrono::nanoseconds>(
          static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_open_logging - st_open_logging)));
      open_logging_cycles += en_open_logging - st_open_logging;
      std_fmt_took
          += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_std_fmt - st_std_fmt)));
      std_cycles += en_std_fmt - st_std_fmt;
      ryu_took += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Bin2Chars::Helpers::Assembly::rdtsc_to_ns(en_ryu - st_ryu)));
      ryu_cycles += en_ryu - st_ryu;

      if(open_logging != std_format)
      {
        // for some reason std::to_chars likes to put a '-' before a nan; the IEEE754 standard doesnt define that so idk
        if(open_logging.contains("nan") && std_format.contains("nan"))
        {
          continue;
        }

        BOOST_CHECK_EQUAL(open_logging, std_format);
        log_str_and_into_hex(LogHexStr("open_logging", open_logging), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

        open_logging = Bin2Chars::Numeric::Floating::DigitsPrecision::ToStr<Bin2Chars::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);

        char buffer[1024];
        d2exp_buffered(static_cast<double>(val), static_cast<uint32_t>(PRECISION), &buffer[0]);

        lim++;
      }
    }
  };

  const auto tester_format_exponential = []<typename T>(const T &bannana, const int &PRECISION)
  {
    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto DENORM = std::numeric_limits<T>::denorm_min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr auto EPS = std::numeric_limits<T>::epsilon();

    std::chrono::nanoseconds open_logging_time{ 0 };
    uint64_t open_logging_cycles{ 0 };
    std::chrono::nanoseconds std_fmt_time{ 0 };
    uint64_t std_fmt_cycles{ 0 };
    std::chrono::nanoseconds ryu_time{ 0 };
    uint64_t ryu_cycles{ 0 };

    // ---- small / subnormal region ----
    lopper_format_exponential(PRECISION, true, T{ 0 }, DENORM, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, MIN, DENORM, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- small normal numbers ----
    lopper_format_exponential(PRECISION, true, MIN, EPS, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, MIN * T{ 10 }, EPS, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- around powers of two ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::ldexp(T{ 1 }, e); // 2^e
      lopper_format_exponential(PRECISION, true, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
      lopper_format_exponential(PRECISION, false, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    }

    // ---- around powers of ten ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = static_cast<T>(std::pow(static_cast<T>(10), e));
      lopper_format_exponential(PRECISION, true, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
      lopper_format_exponential(PRECISION, false, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    }

    // ---- medium magnitude sweeps ----
    lopper_format_exponential(PRECISION, true, static_cast<T>(1), EPS, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, static_cast<T>(100), EPS * static_cast<T>(100), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);
    lopper_format_exponential(PRECISION, true, static_cast<T>(1e6), EPS * static_cast<T>(1e6), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);

    // ---- large numbers ----
    lopper_format_exponential(PRECISION, false, MAX, EPS * MAX, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, false, MAX / static_cast<T>(10), EPS * MAX, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, false, MAX / static_cast<T>(1000), EPS * MAX, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, static_cast<T>(1.234), static_cast<T>(0.0001), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);
    lopper_format_exponential(PRECISION, true, static_cast<T>(123.456), static_cast<T>(0.01), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);
    lopper_format_exponential(PRECISION, false, static_cast<T>(98765.4321), static_cast<T>(0.1), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, static_cast<T>(1.234), static_cast<T>(0.0001), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);
    lopper_format_exponential(PRECISION, true, static_cast<T>(123.456), static_cast<T>(0.01), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);
    lopper_format_exponential(PRECISION, false, static_cast<T>(98765.4321), static_cast<T>(0.1), open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time,
                              ryu_cycles);

    // ---- MASSIVE CHAOS FUZZER ----
    // 1 million purely random bit-patterns per precision level
    // fuzzer_format_exponential(bannana, PRECISION, 1'000'000, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    fuzzer_format_exponential(bannana, PRECISION, 100'000, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    return std::make_tuple(open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
  };

  const auto test_and_benchmark_float = []<typename T>
    requires std::is_floating_point_v<T>
  (const T &, const int &PRECISION)
  {
    const auto float_res = tester_format_exponential(static_cast<T>(0), PRECISION);

    log_time_tables(T{ 0.0 }, "Exponential Formatting ", PRECISION, BenchResult("OpenLogging", std::get<0>(float_res), std::get<1>(float_res)),
                    BenchResult("std::to_chars", std::get<2>(float_res), std::get<3>(float_res)), BenchResult("ryu", std::get<4>(float_res), std::get<5>(float_res)));
  };

} // namespace

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  // floats

  for(int i = 1; i <= 200; i++)
  {
    test_and_benchmark_float(static_cast<float>(0), i);
  }
  // doubles
  // test_and_benchmark_float(static_cast<double>(0), 1);
  // test_and_benchmark_float(static_cast<double>(0), 2);
  // test_and_benchmark_float(static_cast<double>(0), 3);
  // test_and_benchmark_float(static_cast<double>(0), 4);
  // test_and_benchmark_float(static_cast<double>(0), 5);
  // test_and_benchmark_float(static_cast<double>(0), 6);
  // test_and_benchmark_float(static_cast<double>(0), 7);
  // test_and_benchmark_float(static_cast<double>(0), 8);
  // test_and_benchmark_float(static_cast<double>(0), 9);
  // test_and_benchmark_float(static_cast<double>(0), 10);
  // test_and_benchmark_float(static_cast<double>(0), 11);
  // test_and_benchmark_float(static_cast<double>(0), 12);
  // test_and_benchmark_float(static_cast<double>(0), 13);
  // test_and_benchmark_float(static_cast<double>(0), 14);
  // test_and_benchmark_float(static_cast<double>(0), 15);
  // test_and_benchmark_float(static_cast<double>(0), 16);
  // test_and_benchmark_float(static_cast<double>(0), 17);

  // tester_format_exponential(static_cast<long double>(0));
}
///
