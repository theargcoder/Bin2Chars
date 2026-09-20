#define BOOST_TEST_MODULE MagicMathTests
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <locale>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "include/Helpers/Math.hpp"
#include "include/Helpers/Tests.hpp"

using namespace Bin2Chars::Tests;

namespace
{

  template <uint64_t N, typename Type>
  void looper_magic_division(const bool &PLUS, const Type &DELIM, const Type &JUMP, uint64_t &test_ct, uint64_t &err_ct)
  {
    const constexpr auto WISHED_RANGE = 1'000'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    Type divisor = Bin2Chars::Helpers::Math::Constexpr::ipow(Type{ 10 }, N);

    for(Type value = DELIM, errors = 0, max_iter = 0; ((PLUS) ? value < DELIM + RANGE : value > DELIM - RANGE) && errors < MAX_ERRORS && max_iter < RANGE;
        (PLUS) ? value = static_cast<Type>(value + JUMP) : value = static_cast<Type>(value - JUMP), max_iter++)
    {
      test_ct++;
      const Type our_div_10 = Bin2Chars::Helpers::Math::Magic::Division::div_by_10_pow_n<N>(value);
      const Type regular_div_10 = static_cast<Type>(value / divisor);

      if(our_div_10 != regular_div_10)
      {
        BOOST_CHECK_EQUAL(our_div_10, regular_div_10);
        log_str_and_into_hex(LogHexStr("Bin2Chars::Helpers::Math::Magic::div_by_10_denominator", std::to_string(our_div_10)),
                             LogHexStr("regular IDIV got", std::to_string(regular_div_10)));

        errors++;
        err_ct++;
      }
    }
  }

  template <uint64_t N, typename Type>
  void looper_magic_modulus(const bool &PLUS, const Type &DELIM, const Type &JUMP, uint64_t &test_ct, uint64_t &err_ct)
  {
    const constexpr auto WISHED_RANGE = 1'000'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    Type divisor = Bin2Chars::Helpers::Math::Constexpr::ipow(Type{ 10 }, N);

    for(Type value = DELIM, errors = 0, max_iter = 0; ((PLUS) ? value < DELIM + RANGE : value > DELIM - RANGE) && errors < MAX_ERRORS && max_iter < RANGE;
        (PLUS) ? value = static_cast<Type>(value + JUMP) : value = static_cast<Type>(value - JUMP), max_iter++)
    {
      test_ct++;
      const Type our_div_10 = Bin2Chars::Helpers::Math::Magic::Modulo::mod_by_10_pow_n<N>(value);

      const Type regular_div_10 = static_cast<Type>(value % divisor);

      if(our_div_10 != regular_div_10)
      {
        BOOST_CHECK_EQUAL(our_div_10, regular_div_10);
        log_str_and_into_hex(LogHexStr("Bin2Chars::Helpers::Math::Magic::div_by_10_denominator", std::to_string(our_div_10)),
                             LogHexStr("regular IDIV got", std::to_string(regular_div_10)));

        errors++;
        err_ct++;
      }
    }
  }

  template <uint64_t N, typename T>
  auto tester_magic_division(const T &)
  {
    uint64_t test_ct{ 0 }, err_ct{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_magic_division<N, T>(true, MIN, UNIT, test_ct, err_ct);
    looper_magic_division<N, T>(false, MAX, UNIT, test_ct, err_ct);
    looper_magic_division<N, T>(true, T{ 0 }, UNIT, test_ct, err_ct);
    looper_magic_division<N, T>(false, T{ 0 }, UNIT, test_ct, err_ct);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = static_cast<T>(UNIT << e);
      looper_magic_division<N, T>(true, val, UNIT, test_ct, err_ct);
      looper_magic_division<N, T>(false, val, UNIT, test_ct, err_ct);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < static_cast<T>(MAX / 10); val = static_cast<T>(val * 10))
    {
      looper_magic_division<N, T>(true, val, UNIT, test_ct, err_ct);
      looper_magic_division<N, T>(false, val, UNIT, test_ct, err_ct);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_magic_division<N, T>(true, MIN / 2, T{ 123 }, test_ct, err_ct);
      looper_magic_division<N, T>(false, MAX / 2, T{ 123 }, test_ct, err_ct);
    }

    // ---- Randomish coverage ----
    looper_magic_division<N, T>(true, T{ MAX / T{ 10 } }, UNIT, test_ct, err_ct);
    looper_magic_division<N, T>(false, T{ MAX / T{ 10 } * T{ 9 } }, UNIT, test_ct, err_ct);

    return std::make_tuple(test_ct, err_ct);
  }

  template <uint64_t N, typename T>
  auto tester_magic_modulus(const T &) -> auto
  {
    uint64_t test_ct{ 0 }, err_ct{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_magic_modulus<N, T>(true, MIN, UNIT, test_ct, err_ct);
    looper_magic_modulus<N, T>(false, MAX, UNIT, test_ct, err_ct);
    looper_magic_modulus<N, T>(true, T{ 0 }, UNIT, test_ct, err_ct);
    looper_magic_modulus<N, T>(false, T{ 0 }, UNIT, test_ct, err_ct);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = static_cast<T>(UNIT << e);
      looper_magic_modulus<N, T>(true, val, UNIT, test_ct, err_ct);
      looper_magic_modulus<N, T>(false, val, UNIT, test_ct, err_ct);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < static_cast<T>(MAX / 10); val = static_cast<T>(val * 10))
    {
      looper_magic_modulus<N, T>(true, val, UNIT, test_ct, err_ct);
      looper_magic_modulus<N, T>(false, val, UNIT, test_ct, err_ct);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_magic_modulus<N, T>(true, MIN / 2, T{ 123 }, test_ct, err_ct);
      looper_magic_modulus<N, T>(false, MAX / 2, T{ 123 }, test_ct, err_ct);
    }

    // ---- Randomish coverage ----
    looper_magic_modulus<N, T>(true, T{ MAX / T{ 10 } }, UNIT, test_ct, err_ct);
    looper_magic_modulus<N, T>(false, T{ MAX / T{ 10 } * T{ 9 } }, UNIT, test_ct, err_ct);

    return std::make_tuple(test_ct, err_ct);
  }

  template <typename T, std::size_t... Is>
    requires std::is_integral_v<T>
  void test_and_benchmark_div_magic_impl(const T &, std::index_sequence<Is...>)
  {
    const auto exec = [](auto ichar)
    {
      constexpr std::string_view RESET = "\033[0m";
      constexpr std::string_view GREEN = "\033[32m";
      constexpr std::string_view RED = "\033[31m";

      constexpr std::size_t idx = decltype(ichar)::value;

      const auto [test_ct, error_ct] = tester_magic_division<idx, T>(static_cast<T>(0));

      const auto pretty_name = boost::typeindex::type_id<T>().pretty_name();

      const std::locale us_locale("en_US.UTF-8");

      std::cout << std::format(us_locale, "All {:L} modulo by 10^{} tests {} {} {} for type '{}'", test_ct, idx, (error_ct == 0) ? GREEN : RED,
                               (error_ct == 0) ? "passed" : "failed", RESET, pretty_name)
                << "\n";
    };

    (exec(std::integral_constant<std::size_t, Is + 1>{}), ...);
  }

  template <typename T, std::size_t... Is>
    requires std::is_integral_v<T>
  void test_and_benchmark_mod_magic_impl(const T &, std::index_sequence<Is...>)
  {
    const auto exec = [](auto ichar)
    {
      constexpr std::string_view RESET = "\033[0m";
      constexpr std::string_view GREEN = "\033[32m";
      constexpr std::string_view RED = "\033[31m";

      constexpr std::size_t idx = decltype(ichar)::value;

      const auto [test_ct, error_ct] = tester_magic_modulus<idx, T>(static_cast<T>(0));

      const auto pretty_name = boost::typeindex::type_id<T>().pretty_name();

      const std::locale us_locale("en_US.UTF-8");

      std::cout << std::format(us_locale, "All {:L} modulo by 10^{} tests {} {} {} for type '{}'", test_ct, idx, (error_ct == 0) ? GREEN : RED,
                               (error_ct == 0) ? "passed" : "failed", RESET, pretty_name)
                << "\n";
    };

    (exec(std::integral_constant<std::size_t, Is + 1>{}), ...);
  }

  template <typename T>
    requires std::is_integral_v<T>
  void test_and_benchmark_div_magic(const T &)
  {
    test_and_benchmark_div_magic_impl<T>(static_cast<T>(0), std::make_index_sequence<std::numeric_limits<T>::digits10>{});
  }

  template <typename T>
    requires std::is_integral_v<T>
  void test_and_benchmark_mod_magic(const T &)
  {
    test_and_benchmark_mod_magic_impl<T>(static_cast<T>(0), std::make_index_sequence<std::numeric_limits<T>::digits10>{});
  }
} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  test_and_benchmark_div_magic<uint32_t>(0);
  test_and_benchmark_mod_magic<uint32_t>(0);
  test_and_benchmark_div_magic<uint64_t>(0);
  test_and_benchmark_mod_magic<uint64_t>(0);
}
