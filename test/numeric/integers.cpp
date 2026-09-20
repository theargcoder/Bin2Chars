#define BOOST_TEST_MODULE IntegersTest
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <locale>
#include <random>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "include/Algos/Competition.hpp"
#include "include/Algos/Integer.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Tests.hpp"

using namespace Bin2Chars::Tests;

namespace
{
  template <typename Type>
  void fuzzer_ints(const Type & /*unused*/, uint64_t &test_ct, uint64_t &err_ct)
  {
#ifdef NDEBUG
    const constexpr auto SAMPLES = 1'000'000;
#else
    // for debug tests take WAY too long so make em speedy
    const constexpr auto SAMPLES = 200'000;
#endif

    // Fixed seed so test failures are 100% reproducible
    std::mt19937_64 rng(166'543'456);
    std::uniform_int_distribution<Type> dist(0, std::numeric_limits<Type>::max());

    size_t errors = 0;
    const constexpr size_t MAX_ERRORS = 10;
    std::string std_log, std_lib_to_str_log, bin2chars_log;

    for(size_t i = 0; i < SAMPLES && errors < MAX_ERRORS; ++i)
    {
      test_ct++;
      const Type val = dist(rng);

      std_log = Bin2Chars::Numeric::Std::to_string<false, Type>(static_cast<Type>(val), 123);
      std_lib_to_str_log = std::to_string(static_cast<Type>(val));
      bin2chars_log = Bin2Chars::Numeric::Integral::ToStr<Type>(static_cast<Type>(val));

      if(bin2chars_log != std_lib_to_str_log)
      {
#ifdef BIN2CHARS_CIBUILD
        BOOST_REQUIRE(bin2chars_log == std_log);
#else
        BOOST_CHECK_EQUAL(bin2chars_log, std_log);
#endif
        log_str_and_into_hex(LogHexStr("std::to_chars", std_log), LogHexStr("std::to_string", std_lib_to_str_log), LogHexStr("Bin2Chars::ToStr", bin2chars_log));

        errors++;
        err_ct++;
      }
    }
  };

  template <uint64_t N, typename Type>
  void looper_ints(const bool &PLUS, const Type &DELIM, const Type &JUMP, uint64_t &test_ct, uint64_t &err_ct)
  {
#ifdef NDEBUG
    const constexpr auto SAMPLES = 1'000'000;
#else
    // for debug tests take WAY too long so make em speedy
    const constexpr auto SAMPLES = 200'000;
#endif

    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = SAMPLES < MAX_NUM ? static_cast<Type>(SAMPLES) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    uint32_t ext_err = 0;
    uint64_t cycles = 0;

    std::string std_log, std_lib_to_str_log, bin2chars_log;

    while(cycles < SAMPLES && ext_err < MAX_ERRORS)
    {
      cycles += RANGE;
      using LoopType = std::conditional_t<std::is_signed_v<Type>, std::intmax_t, std::uintmax_t>;

      const auto delim = static_cast<LoopType>(DELIM);
      const auto jump = static_cast<LoopType>(JUMP);
      const auto range = static_cast<LoopType>(RANGE);

      constexpr auto LOOP_MAX = std::numeric_limits<LoopType>::max();
      constexpr auto LOOP_MIN = std::numeric_limits<LoopType>::lowest();

      const auto plus_end = delim > LOOP_MAX - range ? LOOP_MAX : delim + range;
      const auto minus_end = delim < LOOP_MIN + range ? LOOP_MIN : delim - range;

      for(LoopType i = delim, int_err = 0, max_iter = 0; (PLUS ? i < plus_end : i > minus_end) && int_err < static_cast<LoopType>(MAX_ERRORS) && max_iter < range;
          (PLUS ? (i > LOOP_MAX - jump ? i = LOOP_MAX : i += jump) : (i < LOOP_MIN + jump ? i = LOOP_MIN : i -= jump)), ++max_iter)
      {
        test_ct++;

        std_log = Bin2Chars::Numeric::Std::to_string<false, Type>(static_cast<Type>(i), 123);

        std_lib_to_str_log = std::to_string(static_cast<Type>(i));

        bin2chars_log = Bin2Chars::Numeric::Integral::ToStr<Type>(static_cast<Type>(i));

        if(bin2chars_log != std_lib_to_str_log)
        {
#ifdef BIN2CHARS_CIBUILD
          BOOST_REQUIRE(bin2chars_log == std_log);
#else
          BOOST_CHECK_EQUAL(bin2chars_log, std_log);
#endif
          log_str_and_into_hex(LogHexStr("std::to_chars", std_log), LogHexStr("std::to_string", std_lib_to_str_log), LogHexStr("Bin2Chars::ToStr", bin2chars_log));

          int_err++;
          ext_err++;
          err_ct++;
        }
      }
    }
  }

  template <uint64_t N, typename T>
  auto tester_ints(const T & /*unused*/)
  {
    uint64_t test_ct{ 0 }, err_ct{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_ints<N>(true, MIN, UNIT, test_ct, err_ct);
    looper_ints<N>(false, MAX, UNIT, test_ct, err_ct);
    looper_ints<N>(true, T{ 0 }, UNIT, test_ct, err_ct);
    looper_ints<N>(false, T{ 0 }, UNIT, test_ct, err_ct);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = static_cast<T>(UNIT << e);
      looper_ints<N>(true, val, UNIT, test_ct, err_ct);
      looper_ints<N>(false, val, UNIT, test_ct, err_ct);
    }

    for(std::intmax_t val = 10; val > 0 && val < static_cast<std::intmax_t>(MAX) / 10; val *= 10)
    {
      looper_ints<N>(true, static_cast<T>(val), UNIT, test_ct, err_ct);
      looper_ints<N>(false, static_cast<T>(val), UNIT, test_ct, err_ct);
    }
    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_ints<N>(true, MIN / 2, T{ 123 }, test_ct, err_ct);
      looper_ints<N>(false, MAX / 2, T{ 123 }, test_ct, err_ct);
    }

    // ---- Randomish coverage ----
    looper_ints<N>(true, T{ MAX / T{ 10 } }, UNIT, test_ct, err_ct);
    looper_ints<N>(false, T{ MAX / T{ 10 } * T{ 9 } }, UNIT, test_ct, err_ct);

    // ---- True Random coverage ----
    fuzzer_ints(static_cast<T>(0), test_ct, err_ct);

    return std::make_tuple(test_ct, err_ct);
  }

  template <typename T>
    requires std::is_integral_v<T>
  void test_and_benchmark_ints(T)
  {
    constexpr std::string_view RESET = "\033[0m";
    constexpr std::string_view GREEN = "\033[32m";
    constexpr std::string_view RED = "\033[31m";

    const auto [test_ct, error_ct] = tester_ints<1>(static_cast<T>(0));

    const auto pretty_name = boost::typeindex::type_id<T>().pretty_name();

    const std::locale us_locale("en_US.UTF-8");

    std::cout << std::format(us_locale, "All {:L} Tests {} {} {} for type '{}'", test_ct, (error_ct == 0) ? GREEN : RED, (error_ct == 0) ? "passed" : "failed", RESET, pretty_name)
              << "\n";
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
