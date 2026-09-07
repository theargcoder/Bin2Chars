#define BOOST_TEST_MODULE ConstantsTests
#include <boost/test/unit_test.hpp>

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/fwd.hpp>

#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Constants/Constants.hpp"
#include "include/Helpers/Simd.hpp"

namespace
{
  namespace Tests
  {
    template <typename T>
      requires(std::is_integral_v<T> || std::is_same_v<T, __uint128_t>)
    int log10(T x)
    {
      int digits = 0;
      while(x >= 10)
      {
        x /= 10;
        ++digits;
      }
      return digits;
    }

    template <typename T>
      requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
    T pow(T base, int exp)
    {
      T result = 1;
      while(exp > 0)
      {
        if(exp & 1)
        {
          result *= base;
        }

        base *= base;
        exp >>= 1;
      }
      return result;
    }
  }

  const auto test_float_table = []<typename T>
    requires std::is_floating_point_v<T>
  (T)
  {
    const auto table = Bin2Chars::Constants::Tables::Floating<T>::DIGITS;
    // Use Boost's 100-decimal-digit precision float for the test bounds
    using BigFloat = boost::multiprecision::cpp_bin_float_100;

    const BigFloat LOG_10_2_BF("0.301029995663981195213738894724493026768"); // Hardcoded for exactness or calculate via Boost

    for(int i = Bin2Chars::Constants::Tables::Floating<float>::MIN_BIN_EXP; i <= Bin2Chars::Constants::Tables::Floating<float>::MAX_BIN_EXP; i++)
    {
      const auto idx = i + Bin2Chars::Constants::Tables::Floating<T>::BIAS;
      const __uint128_t val = static_cast<__uint128_t>(table[idx][0]) * Tests::pow(__uint128_t{ 10 }, 18)
                              + static_cast<__uint128_t>(table[idx][1]) * Tests::pow(__uint128_t{ 10 }, 9) + table[idx][2];
      const int digits = static_cast<int>(boost::multiprecision::log10((BigFloat{ val }))) + 1;
      const BigFloat scale = boost::multiprecision::pow(BigFloat(10), digits - 1);

      // Calculate power of 10 safely
      int32_t p10 = static_cast<int32_t>(std::floor(0.3010299956639812 * i)); // Standard double is fine for the integer exponent

      // Reconstruct approximate value using BigFloat
      BigFloat result = (static_cast<BigFloat>(val) / scale) * boost::multiprecision::pow(BigFloat(10), p10);
      BigFloat expected = boost::multiprecision::pow(BigFloat(2), i);

      BigFloat abs_error = boost::multiprecision::abs(result - expected);
      BigFloat rel_error = abs_error / expected;

      // Bounds testing
      BOOST_CHECK_EQUAL(digits, 9 + 9 + 9);

      // Convert back to standard double for the BOOST_CHECK if needed, or just use Boost's native comparisons.
      BigFloat max_tolerance = boost::multiprecision::pow(BigFloat{ 10.0 }, -1 * (27 - 1));

      BOOST_CHECK_SMALL(rel_error, max_tolerance);

      bool log = !(digits == 27) || !(rel_error <= max_tolerance);

      if(log)
      {
        constexpr int FP_PREC = std::numeric_limits<T>::max_digits10;
        // Note: You may need to format BigFloat as a string for std::format
        std::cout << std::format("table[{:+4}] = {} \t| result {} | expected {}| rel_err {}\n", i, val, result.str(), expected.str(), rel_error.str(), FP_PREC);
      }
    }
  };
} // namespace

BOOST_AUTO_TEST_CASE(test_sig_figs_of_floating_point_v_table)
{
  test_float_table(static_cast<float>(0));
  test_float_table(static_cast<double>(0));
}
