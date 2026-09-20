#include <iostream>
#include <locale>
#include <string_view>
#define BOOST_TEST_MODULE ExponentialNotationTest
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
#include "include/Algos/Floating/Exponential.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Tests.hpp"

using namespace Bin2Chars::Tests;

namespace
{
  template <typename Type>
  void fuzzer_format_exponential(const Type & /*unused*/, const int &PRECISION, uint64_t &test_ct, uint64_t &err_ct)
  {
#ifdef NDEBUG
    const constexpr auto SAMPLES = 200'000;
#else
    // for debug tests take WAY too long so make em speedy
    const constexpr auto SAMPLES = 50'000;
#endif

    using UIntType = std::conditional_t<sizeof(Type) == 4, uint32_t, uint64_t>;

    // Fixed seed so test failures are 100% reproducible
    std::mt19937_64 rng(0xDEADBEEF);
    std::uniform_int_distribution<UIntType> dist(0, std::numeric_limits<UIntType>::max());

    size_t errors = 0;
    const constexpr size_t MAX_ERRORS = 10;

    for(size_t i = 0; i < SAMPLES && errors < MAX_ERRORS; ++i)
    {
      test_ct++;
      const UIntType raw_bits = dist(rng);
      const Type val = std::bit_cast<Type>(raw_bits);

      std::string bin2chars, std_format, ryu;

      bin2chars = Bin2Chars::Numeric::Floating::ExponentialNotation::ToStr(val, PRECISION);

      std_format = Bin2Chars::Numeric::Std::to_string<true>(val, PRECISION);

      ryu = Bin2Chars::Numeric::Ryu::Exponential::ToStr(val, PRECISION);

      if(bin2chars != std_format)
      {
        if(bin2chars.contains("nan") && std_format.contains("nan"))
          continue;

#ifdef BIN2CHARS_CIBUILD
        BOOST_REQUIRE(bin2chars == std_format);
#else
        BOOST_CHECK_EQUAL(bin2chars, std_format);
#endif
        log_str_and_into_hex(LogHexStr("bin2chars", bin2chars), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

        bin2chars = Bin2Chars::Numeric::Floating::ExponentialNotation::ToStr(val, PRECISION);

        char buffer[1024];
        d2exp_buffered(static_cast<double>(val), static_cast<uint32_t>(PRECISION), &buffer[0]);

        errors++;
        err_ct++;
      }
    }
  }

  template <typename Type>
  void lopper_format_exponential(const int &PRECISION, const bool &PLUS, const Type &DELIM, const Type &JUMP, uint64_t &test_ct, uint64_t &err_ct)
  {
#ifdef NDEBUG
    const constexpr auto SAMPLES = 100'000;
#else
    // for debug tests take WAY too long so make em speedy
    const constexpr auto SAMPLES = 25'000;
#endif

    constexpr Type MAX_NUM = std::numeric_limits<Type>::max();
    constexpr Type RANGE = SAMPLES < MAX_NUM ? SAMPLES : MAX_NUM;
    constexpr Type MAX_ERRORS = 10;

    for(Type val = DELIM, errors = 0, max_iter = 0; ((PLUS) ? val < DELIM + RANGE : val > DELIM - RANGE) && errors < MAX_ERRORS && max_iter < RANGE;
        (PLUS) ? val += JUMP : val -= JUMP, max_iter++)
    {
      test_ct++;
      std::string bin2chars, std_format, ryu;

      bin2chars = Bin2Chars::Numeric::Floating::ExponentialNotation::ToStr(val, PRECISION);

      std_format = Bin2Chars::Numeric::Std::to_string<true>(val, PRECISION);

      ryu = Bin2Chars::Numeric::Ryu::Exponential::ToStr(val, PRECISION);

      if(bin2chars != std_format)
      {
        if(bin2chars.contains("nan") && std_format.contains("nan"))
          continue;

#ifdef BIN2CHARS_CIBUILD
        BOOST_REQUIRE(bin2chars == std_format);
#else
        BOOST_CHECK_EQUAL(bin2chars, std_format);
#endif
        log_str_and_into_hex(LogHexStr("bin2chars", bin2chars), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

        bin2chars = Bin2Chars::Numeric::Floating::ExponentialNotation::ToStr(val, PRECISION);

        char buffer[1024];
        d2exp_buffered(static_cast<double>(val), static_cast<uint32_t>(PRECISION), &buffer[0]);
        errors++;
        err_ct++;
      }
    }
  }

  template <typename T>
  auto tester_format_exponential(const T &bannana, const int &PRECISION)
  {
    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto DENORM = std::numeric_limits<T>::denorm_min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr auto EPS = std::numeric_limits<T>::epsilon();

    uint64_t test_ct{ 0 }, err_ct{ 0 };

    // ---- small / subnormal region ----
    lopper_format_exponential(PRECISION, true, T{ 0 }, DENORM, test_ct, err_ct);
    lopper_format_exponential(PRECISION, true, MIN, DENORM, test_ct, err_ct);

    // ---- small normal numbers ----
    lopper_format_exponential(PRECISION, true, MIN, EPS, test_ct, err_ct);
    lopper_format_exponential(PRECISION, true, MIN * T{ 10 }, EPS, test_ct, err_ct);

    // ---- around powers of two ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::ldexp(T{ 1 }, e); // 2^e
      lopper_format_exponential(PRECISION, true, val, EPS * val, test_ct, err_ct);
      lopper_format_exponential(PRECISION, false, val, EPS * val, test_ct, err_ct);
    }

    // ---- around powers of ten ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = static_cast<T>(std::pow(static_cast<T>(10), e));
      lopper_format_exponential(PRECISION, true, val, EPS * val, test_ct, err_ct);
      lopper_format_exponential(PRECISION, false, val, EPS * val, test_ct, err_ct);
    }

    // ---- medium magnitude sweeps ----
    lopper_format_exponential(PRECISION, true, static_cast<T>(1), EPS, test_ct, err_ct);
    lopper_format_exponential(PRECISION, true, static_cast<T>(100), EPS * static_cast<T>(100), test_ct, err_ct);
    lopper_format_exponential(PRECISION, true, static_cast<T>(1e6), EPS * static_cast<T>(1e6), test_ct, err_ct);

    // ---- large numbers ----
    lopper_format_exponential(PRECISION, false, MAX, EPS * MAX, test_ct, err_ct);
    lopper_format_exponential(PRECISION, false, MAX / static_cast<T>(10), EPS * MAX, test_ct, err_ct);
    lopper_format_exponential(PRECISION, false, MAX / static_cast<T>(1000), EPS * MAX, test_ct, err_ct);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, static_cast<T>(1.234), static_cast<T>(0.0001), test_ct, err_ct);
    lopper_format_exponential(PRECISION, true, static_cast<T>(123.456), static_cast<T>(0.01), test_ct, err_ct);
    lopper_format_exponential(PRECISION, false, static_cast<T>(98765.4321), static_cast<T>(0.1), test_ct, err_ct);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, static_cast<T>(1.234), static_cast<T>(0.0001), test_ct, err_ct);
    lopper_format_exponential(PRECISION, true, static_cast<T>(123.456), static_cast<T>(0.01), test_ct, err_ct);
    lopper_format_exponential(PRECISION, false, static_cast<T>(98765.4321), static_cast<T>(0.1), test_ct, err_ct);

    // ---- MASSIVE CHAOS FUZZER ----
    // purely random bit-patterns per precision level
    fuzzer_format_exponential(bannana, PRECISION, test_ct, err_ct);

    return std::make_tuple(test_ct, err_ct);
  }

  template <typename T>
    requires std::is_floating_point_v<T>
  void test_and_benchmark_float(const T &, const int &PRECISION)
  {

    constexpr std::string_view RESET = "\033[0m";
    constexpr std::string_view GREEN = "\033[32m";
    constexpr std::string_view RED = "\033[31m";

    const auto [test_ct, err_ct] = tester_format_exponential(static_cast<T>(0), PRECISION);
    const auto pretty_name = boost::typeindex::type_id<T>().pretty_name();

    const std::locale us_locale("en_US.UTF-8");
    std::cout << std::format(us_locale, "All {:L} exponential formatting tests {} {} {} for type '{}' with precision '{}'", test_ct, (err_ct == 0) ? GREEN : RED,
                             (err_ct == 0) ? "passed" : "failed", RESET, pretty_name, PRECISION)
              << "\n";
  }

} // namespace

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  Bin2Chars::Helpers::Assembly::pin_thread_to_cpu(1);

  // floats // all good
  for(int i = 0; i <= 20; i++)
  {
    test_and_benchmark_float(static_cast<float>(0), i);
  }
  test_and_benchmark_float(static_cast<float>(0), 50);
  test_and_benchmark_float(static_cast<float>(0), 60);
  test_and_benchmark_float(static_cast<float>(0), 70);
  test_and_benchmark_float(static_cast<float>(0), 80);
  test_and_benchmark_float(static_cast<float>(0), 90);
  test_and_benchmark_float(static_cast<float>(0), 100);
  test_and_benchmark_float(static_cast<float>(0), 110);
  test_and_benchmark_float(static_cast<float>(0), 120);
  test_and_benchmark_float(static_cast<float>(0), 130);
  test_and_benchmark_float(static_cast<float>(0), 140);
  test_and_benchmark_float(static_cast<float>(0), 150);
  test_and_benchmark_float(static_cast<float>(0), 160);
  test_and_benchmark_float(static_cast<float>(0), 170);
  test_and_benchmark_float(static_cast<float>(0), 180);

  // doubles // all good
  for(int i = 0; i <= 50; i++)
  {
    test_and_benchmark_float(static_cast<double>(0), i);
  }
  test_and_benchmark_float(static_cast<double>(0), 100);
  test_and_benchmark_float(static_cast<double>(0), 150);
  test_and_benchmark_float(static_cast<double>(0), 200);
  test_and_benchmark_float(static_cast<double>(0), 250);
  test_and_benchmark_float(static_cast<double>(0), 300);
  test_and_benchmark_float(static_cast<double>(0), 350);
  test_and_benchmark_float(static_cast<double>(0), 400);
  test_and_benchmark_float(static_cast<double>(0), 450);
  test_and_benchmark_float(static_cast<double>(0), 500);
  test_and_benchmark_float(static_cast<double>(0), 550);
  test_and_benchmark_float(static_cast<double>(0), 600);
  test_and_benchmark_float(static_cast<double>(0), 650);
  test_and_benchmark_float(static_cast<double>(0), 700);
  test_and_benchmark_float(static_cast<double>(0), 750);
  test_and_benchmark_float(static_cast<double>(0), 800);
}
///
