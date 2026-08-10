#pragma once

#include <bit>
#include <cstdint>
#include <limits>
#include <numbers>
#include <type_traits>

#include "include/Helpers/Assembly.h"

namespace Helpers::Math::Constexpr
{
  template <typename T>
  static constexpr T abs(T x)
  {
    return x < 0 ? -x : x;
  }

  template <typename BaseType, typename ExpType>
    requires std::is_integral_v<ExpType> || std::is_same_v<BaseType, __uint128_t>
  static constexpr BaseType ipow(BaseType base, ExpType exp)
  {
    if(exp < 0)
    {
      if constexpr(std::is_floating_point_v<BaseType>)
      {
        base = BaseType{ 1 } / base;
        exp = -exp;
      }
      else
      {
        return 0;
      }
    }

    BaseType res = 1;
    while(exp > 0)
    {
      if(exp % 2 == 1)
      {
        res *= base;
      }

      exp /= 2;

      // FIX: Prevent squaring base on the final step to avoid constexpr overflow!
      if(exp > 0)
      {
        base *= base;
      }
    }
    return res;
  }

  // --- Natural Logarithm (ln x) ---
  template <typename T>
  static constexpr T ln(T x)
  {
    if(x <= 0)
    {
      return std::numeric_limits<T>::quiet_NaN();
    }
    if(x == 1)
    {
      return 0;
    }

    int k = 0;
    while(x > 1.5)
    {
      x /= 2;
      k++;
    }
    while(x < 0.75)
    {
      x *= 2;
      k--;
    }

    T z = x;
    const constexpr T ln2 = std::numbers::ln2_v<long double>;

    T y = (z - 1) / (z + 1);
    T y2 = y * y;
    T term = y;
    T sum = y;
    for(int i = 3; i < 70; i += 2)
    {
      term *= y2;
      sum += term / i;
    }
    return 2 * sum + (T)k * ln2;
  }

  // --- Exponential (e^x) ---
  template <typename T>
  static constexpr T exp(T x)
  {
    if(x == 0)
    {
      return 1;
    }

    const constexpr T ln2 = std::numbers::ln2_v<T>;
    int k = static_cast<int>(x / ln2);
    T f = x - static_cast<T>(k) * ln2;

    T term = 1;
    T sum = 1;
    for(int i = 1; i < 50; ++i)
    {
      term *= f / i;
      sum += term;
    }

    // FIXED: ipow now handles negatives cleanly
    return sum * ipow(T{ 2 }, k);
  }

  // --- The Full Pow (Floating Point Exponent) ---
  template <typename T>
    requires std::is_floating_point_v<T>
  static constexpr T pow(T base, T exp_val)
  {
    if(exp_val == 0)
    {
      return 1;
    }
    if(base == 0)
    {
      return 0;
    }
    if(base < 0)
    {
      auto i_exp = static_cast<long long>(exp_val);
      if(static_cast<T>(i_exp) == exp_val)
      {
        T res = exp(exp_val * ln(abs(base)));
        return (i_exp % 2 == 0) ? res : -res;
      }
      return std::numeric_limits<T>::quiet_NaN();
    }

    return exp(exp_val * ln(base));
  }

  template <typename BaseType, typename ExpType>
  static consteval BaseType pow(BaseType base, ExpType exp_val)
  {
    return ipow(base, exp_val);
  }

  template <typename T>
    requires(std::is_integral_v<T> || std::is_same_v<T, __uint128_t>)
  static consteval int log10(T x)
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
    requires std::is_integral_v<T>
  static consteval T pow(T base, int exp)
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

  template <typename T>
    requires std::is_floating_point_v<T>
  static consteval T log10(T x)
  {
    if(x <= 0)
    {
      return std::numeric_limits<T>::quiet_NaN();
    }
    if(x == 1)
    {
      return 0;
    }

    // Multiply by 1 / ln(10)
    const constexpr T inv_ln10 = std::numbers::log10e_v<T>;

    return ln(x) * inv_ln10;
  }

  template <typename T>
    requires(std::is_unsigned_v<T> && std::is_integral_v<T>) || std::is_same_v<__uint128_t, T>
  static constexpr bool is_pow10(T n)
  {
    return (n >= 10) && ([]<typename Type> (Type x) constexpr {
        while (x % 10 == 0)
            x /= 10;
        return x == 1;
    })(n);
  }

} // namespace Helpers::Math::Constexpr

namespace Helpers::Math::Magic::Division
{
  template <uint32_t N>
  static auto div_by_10_pow_n(const uint16_t &n)
  {
    static_assert(N != 0, "why divide by 1");
    static_assert(N <= std::numeric_limits<uint16_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1)      { return static_cast<uint16_t>((uint32_t(n) * 0xCCCDU) >> 19); }
    else if constexpr(N == 2) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0x47AFU) >> 16); return (((n - t) >> 1) + t) >> 6;  }
    else if constexpr(N == 3) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0x625U) >> 16); return (((n - t) >> 1) + t) >> 9;   }
    else if constexpr(N == 4) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0xA36FU) >> 16); return (((n - t) >> 1) + t) >> 13; }
    // clang-format on
  }

  template <uint32_t N>
  static auto div_by_10_pow_n_void(uint16_t &n)
  {
    static_assert(N != 0, "why divide by 1");
    static_assert(N <= std::numeric_limits<uint16_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1)      { n = static_cast<uint16_t>((uint32_t(n) * 0xCCCDU) >> 19); }
    else if constexpr(N == 2) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0x47AFU) >> 16); n = (((n - t) >> 1) + t) >> 6;  }
    else if constexpr(N == 3) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0x625U) >> 16); n = (((n - t) >> 1) + t) >> 9;   }
    else if constexpr(N == 4) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0xA36FU) >> 16); n = (((n - t) >> 1) + t) >> 13; }
    // clang-format on
  }

  template <uint32_t N>
  static auto div_by_10_pow_n(const uint32_t &n)
  {
    static_assert(N != 0, "why divide by 1");
    static_assert(N <= std::numeric_limits<uint32_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    // --- 32-bit divide by 10^p ---
    if constexpr(N == 1) {      return Helpers::Assembly::umulh32(n, 0xCCCCCCCDULL) >> 3U; }
    else if constexpr(N == 2) { return Helpers::Assembly::umulh32(n, 0x51EB851FULL) >> 5U; }
    else if constexpr(N == 3) { return Helpers::Assembly::umulh32(n, 0x10624DD3ULL) >> 6U; }
    else if constexpr(N == 4) { return Helpers::Assembly::umulh32(n, 0xD1B71759ULL) >> 13U; }
    else if constexpr(N == 5) { const uint32_t t = Helpers::Assembly::umulh32(n, 0x4F8B588FULL); return (((n - t) >> 1) + t) >> 16U; }
    else if constexpr(N == 6) { return Helpers::Assembly::umulh32(n, 0x431BDE83ULL) >> 18; }
    else if constexpr(N == 7) { return Helpers::Assembly::umulh32(n, 0x6B5FCA6BULL) >> 22; }
    else if constexpr(N == 8) { return Helpers::Assembly::umulh32(n, 0x55E63B89ULL) >> 25; }
    else if constexpr(N == 9) { const uint32_t t = Helpers::Assembly::umulh32(n, 0x12E0BE83ULL); return (((n - t) >> 1) + t) >> 29U; }

    // clang-format on
  }

  template <uint32_t N>
  static inline void div_by_10_pow_n_void(uint32_t &n)
  {
    static_assert(N != 0, "why divide by 1");
    static_assert(N <= std::numeric_limits<uint32_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1) { n = Helpers::Assembly::umulh32(n, 0xCCCCCCCDULL) >> 3U; }
    else if constexpr(N == 2) { n = Helpers::Assembly::umulh32(n, 0x51EB851FULL) >> 5U; }
    else if constexpr(N == 3) { n = Helpers::Assembly::umulh32(n, 0x10624DD3ULL) >> 6U; }
    else if constexpr(N == 4) { n = Helpers::Assembly::umulh32(n, 0xD1B71759ULL) >> 13U; }
    else if constexpr(N == 5) { const uint32_t t = Helpers::Assembly::umulh32(n, 0x4F8B588FULL); n = (((n - t) >> 1) + t) >> 16U; }
    else if constexpr(N == 6) { n = Helpers::Assembly::umulh32(n, 0x431BDE83ULL) >> 18; }
    else if constexpr(N == 7) { n = Helpers::Assembly::umulh32(n, 0x6B5FCA6BULL) >> 22; }
    else if constexpr(N == 8) { n = Helpers::Assembly::umulh32(n, 0x55E63B89ULL) >> 25; }
    else if constexpr(N == 9) { const uint32_t t = Helpers::Assembly::umulh32(n, 0x12E0BE83ULL); n = (((n - t) >> 1) + t) >> 29U; }
    // clang-format on
  }

  template <uint32_t DIV>
  static inline auto div_by_10_template(const uint32_t &numerator)
  {
    static_assert(Helpers::Math::Constexpr::is_pow10(DIV), "only powers of 10 supported");
    static_assert(DIV != 0, "cant divide by 0");
    static_assert(DIV != 1U, "why are you dividing by 1");
    // clang-format off
    if constexpr(DIV <= 10) { return div_by_10_pow_n<1>(numerator); }
    else if constexpr(DIV <= 100) { return div_by_10_pow_n<2>(numerator); }
    else if constexpr(DIV <= 1'000) { return div_by_10_pow_n<3>(numerator); }
    else if constexpr(DIV <= 10'000) { return div_by_10_pow_n<4>(numerator); }
    else if constexpr(DIV <= 100'000) { return div_by_10_pow_n<5>(numerator); }
    else if constexpr(DIV <= 1'000'000) { return div_by_10_pow_n<6>(numerator); }
    else if constexpr(DIV <= 10'000'000) { return div_by_10_pow_n<7>(numerator); }
    else if constexpr(DIV <= 100'000'000) { return div_by_10_pow_n<8>(numerator); }
    else if constexpr(DIV <= 1'000'000'000) { return div_by_10_pow_n<9>(numerator); }
    else { return numerator; }
    // clang-format on
  }

  template <typename Type>
    requires std::is_same_v<uint32_t, Type>
  static auto div_by_10_denominator(const Type &numerator, const Type &denominator)
  {
    // clang-format off
    if(denominator <= 10) { return div_by_10_pow_n<1>(numerator); }
    else if(denominator <= 100) { return div_by_10_pow_n<2>(numerator); }
    else if(denominator <= 1'000) { return div_by_10_pow_n<3>(numerator); }
    else if(denominator <= 10'000) { return div_by_10_pow_n<4>(numerator); }
    else if(denominator <= 100'000) { return div_by_10_pow_n<5>(numerator); }
    else if(denominator <= 1'000'000) { return div_by_10_pow_n<6>(numerator); }
    else if(denominator <= 10'000'000) { return div_by_10_pow_n<7>(numerator); }
    else if(denominator <= 100'000'000) { return div_by_10_pow_n<8>(numerator); }
    else if(denominator <= 1'000'000'000) { return div_by_10_pow_n<9>(numerator); }
    else { return std::remove_cvref_t<decltype(numerator)>{0}; }
    // clang-format on
  }

  template <uint64_t N>
  static inline auto div_by_10_pow_n(const uint64_t &n)
  {
    static_assert(N != 0, "why divide by 1");
    static_assert(N <= std::numeric_limits<uint64_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1) { return Helpers::Assembly::umulh64(n , 0xCCCCCCCCCCCCCCCDULL) >> 3; }
    else if constexpr(N == 2) { uint64_t t = Helpers::Assembly::umulh64(n , 0x47AE147AE147AE15ULL); return (((n - t) >> 1) + t) >> 6; }
    else if constexpr(N == 3) { uint64_t t = Helpers::Assembly::umulh64(n , 0x624DD2F1A9FBE77ULL); return (((n - t) >> 1) + t) >> 9; }
    else if constexpr(N == 4) { return Helpers::Assembly::umulh64(n , 0x346DC5D63886594BULL) >> 11; }
    else if constexpr(N == 5) { uint64_t t = Helpers::Assembly::umulh64(n , 0x4F8B588E368F0847ULL); return (((n - t) >> 1) + t) >> 16; }
    else if constexpr(N == 6) { return Helpers::Assembly::umulh64(n , 0x431BDE82D7B634DBULL) >> 18U; }
    else if constexpr(N == 7) { return Helpers::Assembly::umulh64(n , 0xD6BF94D5E57A42BDULL) >> 23U; }
    else if constexpr(N == 8) { return Helpers::Assembly::umulh64(n , 0xABCC77118461CEFDULL) >> 26U; }
    else if constexpr(N == 9) { uint64_t t = Helpers::Assembly::umulh64(n, 0x12E0BE826D694B2FULL); return (((n - t) >> 1) + t) >> 29; }
    else if constexpr(N == 10) { return Helpers::Assembly::umulh64(n, 0xDBE6FECEBDEDD5BFULL) >> 33U; }
    else if constexpr(N == 11) { return Helpers::Assembly::umulh64(n, 0xAFEBFF0BCB24AAFFULL) >> 36U; }
    else if constexpr(N == 12) { return Helpers::Assembly::umulh64(n, 0x232F33025BD42233ULL) >> 37U; }
    else if constexpr(N == 13) { return Helpers::Assembly::umulh64(n, 0x384B84D092ED0385ULL) >> 41U; }
    else if constexpr(N == 14) { return Helpers::Assembly::umulh64(n, 0xB424DC35095CD81ULL) >> 42U; }
    else if constexpr(N == 15) { uint64_t t = Helpers::Assembly::umulh64(n, 0x203AF9EE756159B3ULL); return (((n - t) >> 1) + t) >> 49; }
    else if constexpr(N == 16) { return Helpers::Assembly::umulh64(n, 0x39A5652FB1137857ULL) >> 51U; }
    else if constexpr(N == 17) { uint64_t t = Helpers::Assembly::umulh64(n, 0x70EF54646D496893ULL); return (((n - t) >> 1) + t) >> 56; }
    else if constexpr(N == 18) { uint64_t t = Helpers::Assembly::umulh64(n, 0x2725DD1D243ABA0FULL); return (((n - t) >> 1) + t) >> 59; }
    else if constexpr(N == 19) { return Helpers::Assembly::umulh64(n, 0x760F253EDB4AB0D3ULL) >> 62U; }
    // clang-format on
  }

  template <uint64_t N>
  static inline void div_by_10_pow_n_void(uint64_t &n)
  {
    static_assert(N != 0, "why divide by 1");
    static_assert(N <= std::numeric_limits<uint64_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1) { n = Helpers::Assembly::umulh64(n , 0xCCCCCCCCCCCCCCCDULL) >> 3; }
    else if constexpr(N == 2) { uint64_t t = Helpers::Assembly::umulh64(n , 0x47AE147AE147AE15ULL); n = (((n - t) >> 1) + t) >> 6; }
    else if constexpr(N == 3) { uint64_t t = Helpers::Assembly::umulh64(n , 0x624DD2F1A9FBE77ULL); n = (((n - t) >> 1) + t) >> 9; }
    else if constexpr(N == 4) { n = Helpers::Assembly::umulh64(n , 0x346DC5D63886594BULL) >> 11; }
    else if constexpr(N == 5) { uint64_t t = Helpers::Assembly::umulh64(n , 0x4F8B588E368F0847ULL); n = (((n - t) >> 1) + t) >> 16; }
    else if constexpr(N == 6) { n = Helpers::Assembly::umulh64(n , 0x431BDE82D7B634DBULL) >> 18U; }
    else if constexpr(N == 7) { n = Helpers::Assembly::umulh64(n , 0xD6BF94D5E57A42BDULL) >> 23U; }
    else if constexpr(N == 8) { n = Helpers::Assembly::umulh64(n , 0xABCC77118461CEFDULL) >> 26U; }
    else if constexpr(N == 9) { uint64_t t = Helpers::Assembly::umulh64(n, 0x12E0BE826D694B2FULL); n = (((n - t) >> 1) + t) >> 29; }
    else if constexpr(N == 10) { n = Helpers::Assembly::umulh64(n, 0xDBE6FECEBDEDD5BFULL) >> 33U; }
    else if constexpr(N == 11) { n = Helpers::Assembly::umulh64(n, 0xAFEBFF0BCB24AAFFULL) >> 36U; }
    else if constexpr(N == 12) { n = Helpers::Assembly::umulh64(n, 0x232F33025BD42233ULL) >> 37U; }
    else if constexpr(N == 13) { n = Helpers::Assembly::umulh64(n, 0x384B84D092ED0385ULL) >> 41U; }
    else if constexpr(N == 14) { n = Helpers::Assembly::umulh64(n, 0xB424DC35095CD81ULL) >> 42U; }
    else if constexpr(N == 15) { uint64_t t = Helpers::Assembly::umulh64(n, 0x203AF9EE756159B3ULL); n = (((n - t) >> 1) + t) >> 49; }
    else if constexpr(N == 16) { n = Helpers::Assembly::umulh64(n, 0x39A5652FB1137857ULL) >> 51U; }
    else if constexpr(N == 17) { uint64_t t = Helpers::Assembly::umulh64(n, 0x70EF54646D496893ULL); n = (((n - t) >> 1) + t) >> 56; }
    else if constexpr(N == 18) { uint64_t t = Helpers::Assembly::umulh64(n, 0x2725DD1D243ABA0FULL); n = (((n - t) >> 1) + t) >> 59; }
    else if constexpr(N == 19) { n = Helpers::Assembly::umulh64(n, 0x760F253EDB4AB0D3ULL) >> 62U; }
    // clang-format on
  }
  template <uint64_t DIV>
  static inline auto div_by_10_pow_template(const uint64_t &n)
  {
    static_assert(Helpers::Math::Constexpr::is_pow10(DIV), "only powers of 10 supported");
    static_assert(DIV != 0, "cant divide by 0");
    static_assert(DIV != 1U, "why are you dividing by 1");
    // clang-format off
    if constexpr (DIV <= 10) {  return div_by_10_pow_n<1>(n); }
    else if constexpr (DIV <= 100) {  return div_by_10_pow_n<2>(n); }
    else if constexpr (DIV <= 1'000ULL) {  return div_by_10_pow_n<2>(n); }
    else if constexpr (DIV <= 10'000ULL) {  return div_by_10_pow_n<3>(n); }
    else if constexpr (DIV <= 100'000ULL) {  return div_by_10_pow_n<4>(n); }
    else if constexpr (DIV <= 1'000'000ULL) {  return div_by_10_pow_n<5>(n); }
    else if constexpr (DIV <= 10'000'000ULL) {  return div_by_10_pow_n<6>(n); }
    else if constexpr (DIV <= 100'000'000ULL) {  return div_by_10_pow_n<7>(n); }
    else if constexpr (DIV <= 1'000'000'000ULL) {  return div_by_10_pow_n<8>(n); }
    else if constexpr (DIV <= 10'000'000'000ULL) {  return div_by_10_pow_n<9>(n); }
    else if constexpr (DIV <= 100'000'000'000ULL) {  return div_by_10_pow_n<10>(n); }
    else if constexpr (DIV <= 1'000'000'000'000ULL) {  return div_by_10_pow_n<11>(n); }
    else if constexpr (DIV <= 10'000'000'000'000ULL) {  return div_by_10_pow_n<12>(n); }
    else if constexpr (DIV <= 100'000'000'000'000ULL) {  return div_by_10_pow_n<13>(n); }
    else if constexpr (DIV <= 1'000'000'000'000'000ULL) {  return div_by_10_pow_n<14>(n); }
    else if constexpr (DIV <= 10'000'000'000'000'000ULL) {  return div_by_10_pow_n<15>(n); }
    else if constexpr (DIV <= 100'000'000'000'000'000ULL) {  return div_by_10_pow_n<16>(n); }
    else if constexpr (DIV <= 1'000'000'000'000'000'000ULL) {  return div_by_10_pow_n<17>(n); }
    else if constexpr (DIV <= 10'000'000'000'000'000'000ULL) {  return div_by_10_pow_n<18>(n); }
    else {  return div_by_10_pow_n<19>(n); }
    // clang-format on
  }

  template <typename Type>
    requires std::is_same_v<uint64_t, Type>
  static auto div_by_10_denominator(const Type &numerator, const Type &denominator)
  {
    // clang-format off
    if (denominator <= 10) {  return div_by_10_pow_n<1>(numerator); }
    else if (denominator <= 100) {  return div_by_10_pow_n<2>(numerator); }
    else if (denominator <= 1'000ULL) {  return div_by_10_pow_n<2>(numerator); }
    else if (denominator <= 10'000ULL) {  return div_by_10_pow_n<3>(numerator); }
    else if (denominator <= 100'000ULL) {  return div_by_10_pow_n<4>(numerator); }
    else if (denominator <= 1'000'000ULL) {  return div_by_10_pow_n<5>(numerator); }
    else if (denominator <= 10'000'000ULL) {  return div_by_10_pow_n<6>(numerator); }
    else if (denominator <= 100'000'000ULL) {  return div_by_10_pow_n<7>(numerator); }
    else if (denominator <= 1'000'000'000ULL) {  return div_by_10_pow_n<8>(numerator); }
    else if (denominator <= 10'000'000'000ULL) {  return div_by_10_pow_n<9>(numerator); }
    else if (denominator <= 100'000'000'000ULL) {  return div_by_10_pow_n<10>(numerator); }
    else if (denominator <= 1'000'000'000'000ULL) {  return div_by_10_pow_n<11>(numerator); }
    else if (denominator <= 10'000'000'000'000ULL) {  return div_by_10_pow_n<12>(numerator); }
    else if (denominator <= 100'000'000'000'000ULL) {  return div_by_10_pow_n<13>(numerator); }
    else if (denominator <= 1'000'000'000'000'000ULL) {  return div_by_10_pow_n<14>(numerator); }
    else if (denominator <= 10'000'000'000'000'000ULL) {  return div_by_10_pow_n<15>(numerator); }
    else if (denominator <= 100'000'000'000'000'000ULL) {  return div_by_10_pow_n<16>(numerator); }
    else if (denominator <= 1'000'000'000'000'000'000ULL) {  return div_by_10_pow_n<17>(numerator); }
    else if (denominator <= 10'000'000'000'000'000'000ULL) {  return div_by_10_pow_n<18>(numerator); }
    else {  return div_by_10_pow_n<19>(numerator); }
    // clang-format on
  }

  template <typename T>
  static auto top_digit(const T &input)
  {
    return div_by_10_denominator(input, input / 10);
  }

} // namespace Helpers::Math::Magic::Division

namespace Helpers::Math::Magic::Modulo
{
  template <uint32_t N>
  static inline auto mod_by_10_pow_n(const uint16_t &n)
  {
    static_assert(N != 0, "why modide by 1");
    static_assert(N <= std::numeric_limits<uint16_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1) { return static_cast<uint16_t>(n - ((uint32_t(n) * 0xCCCDU) >> 19) * 10U); }
    else if constexpr(N == 2) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0x47AFU) >> 16); const uint16_t q = (((n - t) >> 1) + t) >> 6; return static_cast<uint16_t>(n - (q * 100U)); }
    else if constexpr(N == 3) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0x625U) >> 16); const uint16_t q = (((n - t) >> 1) + t) >> 9; return static_cast<uint16_t>(n - (q * 1000U)); }
    else if constexpr(N == 4) { const uint16_t t = static_cast<uint16_t>((uint32_t(n) * 0xA36FU) >> 16); const uint16_t q = (((n - t) >> 1) + t) >> 13; return static_cast<uint16_t>(n - (q * 10000U)); }
    // clang-format on
  }

  template <uint32_t N>
  static inline auto mod_by_10_pow_n_void(uint16_t &quotient, uint16_t &remainder)
  {
    static_assert(N != 0, "why modide by 1");
    static_assert(N <= std::numeric_limits<uint16_t>::digits10, "10 ^exp is greater that num of digits");

    const uint16_t A = quotient;
    // clang-format off
    if constexpr(N == 1) { Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(quotient); remainder = A - (quotient * 10U); }
    else if constexpr(N == 2) { Helpers::Math::Magic::Division::div_by_10_pow_n_void<2>(quotient); remainder = A - (quotient * 100U); }
    else if constexpr(N == 3) { Helpers::Math::Magic::Division::div_by_10_pow_n_void<3>(quotient); remainder = A - (quotient * 1000U); }
    else if constexpr(N == 4) { Helpers::Math::Magic::Division::div_by_10_pow_n_void<4>(quotient); remainder = A - (quotient * 10000U); }
    // clang-format on
  }

  template <uint32_t N>
  static inline auto mod_by_10_pow_n(const uint32_t &n)
  {
    static_assert(N != 0, "why modide by 1");
    static_assert(N <= std::numeric_limits<uint32_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
    if constexpr(N == 1) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10U)); }
    else if constexpr(N == 2) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100U)); }
    else if constexpr(N == 3) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000U)); }
    else if constexpr(N == 4) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000U)); }
    else if constexpr(N == 5) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000U)); }
    else if constexpr(N == 6) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000U)); }
    else if constexpr(N == 7) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000000U)); }
    else if constexpr(N == 8) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000000U)); }
    else if constexpr(N == 9) { return static_cast<uint32_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000000U)); }
    // clang-format on
  }

  template <uint32_t N, typename Type>
    requires std::is_unsigned_v<Type> && (sizeof(Type) >= sizeof(uint32_t))
  static inline auto mod_by_10_pow_n_void(uint32_t &quotient, Type &remainder)
  {
    static_assert(N != 0, "why modide by 1");
    static_assert(N <= std::numeric_limits<uint32_t>::digits10, "10 ^exp is greater that num of digits");

    const uint32_t A = quotient;
    // clang-format off
    if constexpr(N == 1)       {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10U); }
    else if constexpr(N == 2)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100U); }
    else if constexpr(N == 3)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000U); }
    else if constexpr(N == 4)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000U); }
    else if constexpr(N == 5)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000U); }
    else if constexpr(N == 6)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000U); }
    else if constexpr(N == 7)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000000U); }
    else if constexpr(N == 8)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000000U); }
    else if constexpr(N == 9)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000000U); }
    // clang-format on
  }

  static auto mod_by_10_denominator(const uint32_t &numerator, const uint32_t &denominator)
  {
    // clang-format off
    if(denominator <= 10) { return mod_by_10_pow_n<1>(numerator); }
    else if(denominator <= 100) { return mod_by_10_pow_n<2>(numerator); }
    else if(denominator <= 1'000) { return mod_by_10_pow_n<3>(numerator); }
    else if(denominator <= 10'000) { return mod_by_10_pow_n<4>(numerator); }
    else if(denominator <= 100'000) { return mod_by_10_pow_n<5>(numerator); }
    else if(denominator <= 1'000'000) { return mod_by_10_pow_n<6>(numerator); }
    else if(denominator <= 10'000'000) { return mod_by_10_pow_n<7>(numerator); }
    else if(denominator <= 100'000'000) { return mod_by_10_pow_n<8>(numerator); }
    else if(denominator <= 1'000'000'000) { return mod_by_10_pow_n<9>(numerator); }
    else { return std::remove_cvref_t<decltype(numerator)>{0}; }
    // clang-format on
  }

  template <uint32_t MOD>
  static inline auto mod_by_10_template(const uint32_t &numerator)
  {
    static_assert(Helpers::Math::Constexpr::is_pow10(MOD), "only powers of 10 supported");
    static_assert(MOD != 0, "cant mod by 0");
    static_assert(MOD != 1U, "why are you mod by 1");
    // clang-format off
    if constexpr(MOD <= 10) { return mod_by_10_pow_n<1>(numerator); }
    else if constexpr(MOD <= 100) { return mod_by_10_pow_n<2>(numerator); }
    else if constexpr(MOD <= 1'000) { return mod_by_10_pow_n<3>(numerator); }
    else if constexpr(MOD <= 10'000) { return mod_by_10_pow_n<4>(numerator); }
    else if constexpr(MOD <= 100'000) { return mod_by_10_pow_n<5>(numerator); }
    else if constexpr(MOD <= 1'000'000) { return mod_by_10_pow_n<6>(numerator); }
    else if constexpr(MOD <= 10'000'000) { return mod_by_10_pow_n<7>(numerator); }
    else if constexpr(MOD <= 100'000'000) { return mod_by_10_pow_n<8>(numerator); }
    else if constexpr(MOD <= 1'000'000'000) { return mod_by_10_pow_n<9>(numerator); }
    else { return std::remove_cvref_t<decltype(numerator)>{0}; }
    // clang-format on
  }

  template <uint32_t N>
  static inline auto mod_by_10_pow_n(const uint64_t &n)
  {
    static_assert(N != 0, "why mod by 1");
    static_assert(N <= std::numeric_limits<uint64_t>::digits10, "10 ^exp is greater that num of digits");

    // clang-format off
      if constexpr(N == 1)      { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10ULL)); }
      else if constexpr(N == 2) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100ULL)); }
      else if constexpr(N == 3) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000ULL)); }
      else if constexpr(N == 4) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000ULL)); }
      else if constexpr(N == 5) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000ULL)); }
      else if constexpr(N == 6) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000ULL)); }
      else if constexpr(N == 7) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000000ULL)); }
      else if constexpr(N == 8) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000000ULL)); }
      else if constexpr(N == 9) { return static_cast<uint64_t>(n -  (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000000ULL)); }
      else if constexpr(N == 10) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000000000ULL)); }
      else if constexpr(N == 11) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000000000ULL)); }
      else if constexpr(N == 12) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000000000ULL)); }
      else if constexpr(N == 13) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000000000000ULL)); }
      else if constexpr(N == 14) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000000000000ULL)); }
      else if constexpr(N == 15) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000000000000ULL)); }
      else if constexpr(N == 16) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000000000000000ULL)); }
      else if constexpr(N == 17) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 100000000000000000ULL)); }
      else if constexpr(N == 18) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 1000000000000000000ULL)); }
      else if constexpr(N == 19) { return static_cast<uint64_t>(n - (Helpers::Math::Magic::Division::div_by_10_pow_n<N>(n) * 10000000000000000000ULL)); }
    // clang-format on
  }

  template <uint32_t N, typename Type>
    requires std::is_unsigned_v<Type> && (sizeof(Type) >= sizeof(uint64_t))
  static inline auto mod_by_10_pow_n_void(uint64_t &quotient, Type &remainder)
  {

    static_assert(N != 0, "why mod by 1");
    static_assert(N <= std::numeric_limits<uint64_t>::digits10, "10 ^exp is greater that num of digits");

    const uint64_t A = quotient;

    // clang-format off
      if constexpr(N == 1)       {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10ULL); }
      else if constexpr(N == 2)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100ULL); }
      else if constexpr(N == 3)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000ULL); }
      else if constexpr(N == 4)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000ULL); }
      else if constexpr(N == 5)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000ULL); }
      else if constexpr(N == 6)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000ULL); }
      else if constexpr(N == 7)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000000ULL); }
      else if constexpr(N == 8)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000000ULL); }
      else if constexpr(N == 9)  {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000000ULL); }
      else if constexpr(N == 10) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000000000ULL); }
      else if constexpr(N == 11) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000000000ULL); }
      else if constexpr(N == 12) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000000000ULL); }
      else if constexpr(N == 13) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000000000000ULL); }
      else if constexpr(N == 14) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000000000000ULL); }
      else if constexpr(N == 15) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000000000000ULL); }
      else if constexpr(N == 16) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000000000000000ULL); }
      else if constexpr(N == 17) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 100000000000000000ULL); }
      else if constexpr(N == 18) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 1000000000000000000ULL); }
      else if constexpr(N == 19) {Helpers::Math::Magic::Division::div_by_10_pow_n_void<N>(quotient); remainder = A - (quotient * 10000000000000000000ULL); }
    // clang-format on
  }

  template <uint64_t MOD>
  static inline auto mod_by_10_template(const uint64_t &numerator)
  {
    static_assert(Helpers::Math::Constexpr::is_pow10(MOD), "only powers of 10 supported");
    static_assert(MOD != 0, "cant mod by 0");
    static_assert(MOD != 1U, "why are you mod by 1");

    // clang-format off
    if constexpr (MOD <= 10) {  return mod_by_10_pow_n<1>(numerator); }
    else if constexpr (MOD <= 100) {  return mod_by_10_pow_n<2>(numerator); }
    else if constexpr (MOD <= 1'000ULL) {  return mod_by_10_pow_n<2>(numerator); }
    else if constexpr (MOD <= 10'000ULL) {  return mod_by_10_pow_n<3>(numerator); }
    else if constexpr (MOD <= 100'000ULL) {  return mod_by_10_pow_n<4>(numerator); }
    else if constexpr (MOD <= 1'000'000ULL) {  return mod_by_10_pow_n<5>(numerator); }
    else if constexpr (MOD <= 10'000'000ULL) {  return mod_by_10_pow_n<6>(numerator); }
    else if constexpr (MOD <= 100'000'000ULL) {  return mod_by_10_pow_n<7>(numerator); }
    else if constexpr (MOD <= 1'000'000'000ULL) {  return mod_by_10_pow_n<8>(numerator); }
    else if constexpr (MOD <= 10'000'000'000ULL) {  return mod_by_10_pow_n<9>(numerator); }
    else if constexpr (MOD <= 100'000'000'000ULL) {  return mod_by_10_pow_n<10>(numerator); }
    else if constexpr (MOD <= 1'000'000'000'000ULL) {  return mod_by_10_pow_n<11>(numerator); }
    else if constexpr (MOD <= 10'000'000'000'000ULL) {  return mod_by_10_pow_n<12>(numerator); }
    else if constexpr (MOD <= 100'000'000'000'000ULL) {  return mod_by_10_pow_n<13>(numerator); }
    else if constexpr (MOD <= 1'000'000'000'000'000ULL) {  return mod_by_10_pow_n<14>(numerator); }
    else if constexpr (MOD <= 10'000'000'000'000'000ULL) {  return mod_by_10_pow_n<15>(numerator); }
    else if constexpr (MOD <= 100'000'000'000'000'000ULL) {  return mod_by_10_pow_n<16>(numerator); }
    else if constexpr (MOD <= 1'000'000'000'000'000'000ULL) {  return mod_by_10_pow_n<17>(numerator); }
    else if constexpr (MOD <= 10'000'000'000'000'000'000ULL) {  return mod_by_10_pow_n<18>(numerator); }
    else {  return mod_by_10_pow_n<19>(numerator); }
    // clang-format on
  }

  static auto mod_by_10_denominator(const uint64_t &numerator, const uint64_t &denominator)
  {
    // clang-format off
    if (denominator <= 10) {  return mod_by_10_pow_n<1>(numerator); }
    else if (denominator <= 100) {  return mod_by_10_pow_n<2>(numerator); }
    else if (denominator <= 1'000ULL) {  return mod_by_10_pow_n<2>(numerator); }
    else if (denominator <= 10'000ULL) {  return mod_by_10_pow_n<3>(numerator); }
    else if (denominator <= 100'000ULL) {  return mod_by_10_pow_n<4>(numerator); }
    else if (denominator <= 1'000'000ULL) {  return mod_by_10_pow_n<5>(numerator); }
    else if (denominator <= 10'000'000ULL) {  return mod_by_10_pow_n<6>(numerator); }
    else if (denominator <= 100'000'000ULL) {  return mod_by_10_pow_n<7>(numerator); }
    else if (denominator <= 1'000'000'000ULL) {  return mod_by_10_pow_n<8>(numerator); }
    else if (denominator <= 10'000'000'000ULL) {  return mod_by_10_pow_n<9>(numerator); }
    else if (denominator <= 100'000'000'000ULL) {  return mod_by_10_pow_n<10>(numerator); }
    else if (denominator <= 1'000'000'000'000ULL) {  return mod_by_10_pow_n<11>(numerator); }
    else if (denominator <= 10'000'000'000'000ULL) {  return mod_by_10_pow_n<12>(numerator); }
    else if (denominator <= 100'000'000'000'000ULL) {  return mod_by_10_pow_n<13>(numerator); }
    else if (denominator <= 1'000'000'000'000'000ULL) {  return mod_by_10_pow_n<14>(numerator); }
    else if (denominator <= 10'000'000'000'000'000ULL) {  return mod_by_10_pow_n<15>(numerator); }
    else if (denominator <= 100'000'000'000'000'000ULL) {  return mod_by_10_pow_n<16>(numerator); }
    else if (denominator <= 1'000'000'000'000'000'000ULL) {  return mod_by_10_pow_n<17>(numerator); }
    else if (denominator <= 10'000'000'000'000'000'000ULL) {  return mod_by_10_pow_n<18>(numerator); }
    else {  return mod_by_10_pow_n<19>(numerator); }
    // clang-format on
  }

} // namespace Helpers::Math::Magic::Modulo

namespace Helpers::Math::Precision
{
  static void truncate_plus_1_quo_rem(uint32_t &quotient, uint32_t &rem, const auto &PRECISION)
  {
    // clang-format off
    switch(PRECISION)
    {
    case 0: { rem = 0; } break;
    case 1: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(quotient, rem); } break;
    case 2: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<2>(quotient, rem); } break;
    case 3: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<3>(quotient, rem); } break;
    case 4: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<4>(quotient, rem); } break;
    case 5: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<5>(quotient, rem); } break;
    case 6: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<6>(quotient, rem); } break;
    case 7: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<7>(quotient, rem); } break;
    case 8: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(quotient, rem); } break;
    case 9: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<9>(quotient, rem); } break;
    default: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<6>(quotient, rem); } break;
    }
    // clang-format on
  }

  static void truncate_plus_1_quo_rem(uint64_t &quotient, uint64_t &rem, const auto &PRECISION)
  {
    // clang-format off
    switch(PRECISION)
    {
    case 0: { rem = 0; } break;
    case 1: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(quotient, rem); } break;
    case 2: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<2>(quotient, rem); } break;
    case 3: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<3>(quotient, rem); } break;
    case 4: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<4>(quotient, rem); } break;
    case 5: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<5>(quotient, rem); } break;
    case 6: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<6>(quotient, rem); } break;
    case 7: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<7>(quotient, rem); } break;
    case 8: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(quotient, rem); } break;
    case 9: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<9>(quotient, rem); } break;
    case 10: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<10>(quotient, rem); } break;
    case 11: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<11>(quotient, rem); } break;
    case 12: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<12>(quotient, rem); } break;
    case 13: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<13>(quotient, rem); } break;
    case 14: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<14>(quotient, rem); } break;
    case 15: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<15>(quotient, rem); } break;
    case 16: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<16>(quotient, rem); } break;
    case 17: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<17>(quotient, rem); } break;
    case 18: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<18>(quotient, rem); } break;
    default: { Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<6>(quotient, rem); } break;
    }
    // clang-format on
  }
} // namespace Helpers::Math::Precision

namespace Helpers::Math::IEEE754
{
  template <typename T>
    requires std::is_floating_point_v<T> && std::numeric_limits<T>::is_iec559
  static bool GetMantissaExponent(const T &input, auto &mantissa, int &exponent) noexcept;

  template <>
  bool GetMantissaExponent<float>(const float &input, uint32_t &mantissa, int &exponent) noexcept
  {
    using underlying = uint32_t;

    static const constexpr uint8_t EXPONENT_ST = 23U;
    static const constexpr uint8_t MANTISSA_SHIFT = 8U;
    static const constexpr uint8_t EXPONENT_LEFT_OFFSET = sizeof(float) * 8 - EXPONENT_ST - 1;
    static const constexpr uint8_t EXPONENT_ALL_BITS_ON = 255U; // as defined in IEEE-754

    static const constexpr int16_t MIN_EXPONENT = std::numeric_limits<float>::min_exponent - std::numeric_limits<float>::digits;
    static const constexpr int16_t EXPONENT_TABLE_OFFSET = std::numeric_limits<double>::min_exponent - std::numeric_limits<double>::digits;
    static const constexpr int16_t EXPONENT_TABLE_BIAS = -EXPONENT_TABLE_OFFSET + MIN_EXPONENT + EXPONENT_ST;

    static const constexpr underlying EXPONENT_ONLY = 0x7F800000U;
    static const constexpr underlying MANTISSA_ONLY = 0x007FFFFFU;
    static const constexpr underlying MANTISSA_IMPLICIT_1 = underlying{ 1 } << EXPONENT_ST;
    static const constexpr underlying SIGN_ONLY = 0x80000000U;

    const auto bits = std::bit_cast<underlying>(input);

    const underlying man = bits & MANTISSA_ONLY;

    const uint8_t exp = ((bits & EXPONENT_ONLY) >> EXPONENT_ST);

    if(exp >= EXPONENT_ALL_BITS_ON) [[unlikely]]
    {
      const underlying SIGN = bits & SIGN_ONLY;
      mantissa = (man == 0) ? (SIGN) ? 2 : 1 : 0;
      return true;
    }

    if(exp > 0) [[likely]]
    {
      mantissa = (man | MANTISSA_IMPLICIT_1) << MANTISSA_SHIFT;
      exponent = exp + EXPONENT_TABLE_BIAS;
    }
    else
    {
      const auto shift_internal = std::countl_zero(man) - EXPONENT_LEFT_OFFSET;

      if(shift_internal <= EXPONENT_ST) [[likely]]
      {
        mantissa = (((man << shift_internal) & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1) << MANTISSA_SHIFT;
        exponent = 1 - shift_internal + EXPONENT_TABLE_BIAS;
      }
      else
      {
        mantissa = std::numeric_limits<std::remove_cvref_t<decltype(mantissa)>>::max();
        return true;
      }
    }

    return false;
  }

  template <>
  bool GetMantissaExponent<double>(const double &input, uint64_t &mantissa, int &exponent) noexcept
  {
    using underlying = uint64_t;

    static const constexpr uint8_t NUM_OF_BITS = sizeof(double) * 8U;
    static const constexpr uint8_t EXPONENT_ST = 52U;
    static const constexpr uint8_t MANTISSA_SHIFT = 11U;
    static const constexpr uint8_t EXPONENT_LEFT_OFFSET = NUM_OF_BITS - EXPONENT_ST - 1;

    static const constexpr uint16_t EXPONENT_ALL_BITS_ON = 2047U; // as defined in IEEE-754

    static const constexpr underlying EXPONENT_ONLY = 0x7FF0000000000000ULL;
    static const constexpr underlying MANTISSA_ONLY = 0x000FFFFFFFFFFFFFULL;
    static const constexpr underlying MANTISSA_IMPLICIT_1 = underlying{ 1 } << EXPONENT_ST;

    static const constexpr underlying SIGN_ONLY = 0x8000000000000000ULL;

    const auto bits = std::bit_cast<underlying>(input);

    const underlying man = bits & MANTISSA_ONLY;

    const uint16_t exp = ((bits & EXPONENT_ONLY) >> EXPONENT_ST);

    if(exp >= EXPONENT_ALL_BITS_ON) [[unlikely]]
    {
      const underlying SIGN = bits & SIGN_ONLY;
      mantissa = (man == 0) ? (SIGN) ? 2 : 1 : 0;
      return true;
    }

    if(exp > 0) [[likely]]
    {
      mantissa = (man | MANTISSA_IMPLICIT_1) << MANTISSA_SHIFT;
      exponent = exp + EXPONENT_ST;
    }
    else
    {
      const auto shift_internal = std::countl_zero(man) - EXPONENT_LEFT_OFFSET;

      if(shift_internal <= EXPONENT_ST) [[likely]]
      {
        mantissa = (((man << shift_internal) & MANTISSA_ONLY) | MANTISSA_IMPLICIT_1) << MANTISSA_SHIFT;
        exponent = 1 + EXPONENT_ST - shift_internal;
      }
      else
      {
        mantissa = std::numeric_limits<std::remove_cvref_t<decltype(mantissa)>>::max();
        return true;
      }
    }

    return false;
  }

  namespace Exponential
  {
    template <typename T>
      requires std::is_floating_point_v<T> && std::numeric_limits<T>::is_iec559
    static inline auto Multiply(const auto &mantissa, const uint32_t *table, auto &result, auto &next_9_digits) noexcept;

    template <>
    inline auto Multiply<float>(const uint32_t &mantissa, const uint32_t *table, uint32_t &result, uint32_t &next_9_digits) noexcept
    {
      const constexpr uint32_t DEC9 = 1'000'000'000U;

      const uint64_t u64_prod_0 = static_cast<uint64_t>(mantissa) * table[0];
      const uint64_t u64_prod_1 = static_cast<uint64_t>(mantissa) * table[1];

      const auto u32low_prod_0 = static_cast<uint32_t>(u64_prod_0);
      const auto u32low_1e9 = Helpers::Assembly::umulh32(u32low_prod_0, DEC9);
      const auto u32hig_prod_1 = static_cast<uint32_t>(u64_prod_1 >> 32U);

      result = u64_prod_0 >> 32U;
      next_9_digits = u32hig_prod_1 + u32low_1e9;

      while(next_9_digits >= DEC9)
      {
        result++;
        next_9_digits -= DEC9;
      }
    }

    template <>
    inline auto Multiply<double>(const uint64_t &mantissa, const uint32_t *table, uint64_t &result, uint32_t &next_9_digits) noexcept
    {
      const constexpr uint64_t DEC9 = 1'000'000'000ULL;

      const uint64_t m_high_mid = static_cast<uint64_t>(table[0]) * DEC9 + table[1];
      const auto p_low_top = static_cast<uint32_t>(Helpers::Assembly::umulh64(mantissa, table[2]));

      const __uint128_t u128_prod = (__uint128_t)mantissa * m_high_mid;
      const auto p_hi_mid_rem_times_1e9 = static_cast<uint32_t>(Helpers::Assembly::umulh64(u128_prod, DEC9));

      result = u128_prod >> 64U;
      next_9_digits = p_low_top + p_hi_mid_rem_times_1e9;

      while(next_9_digits >= DEC9)
      {
        result++;
        next_9_digits -= DEC9;
      }
    }
  } // namespace Exponential

  namespace Fixed
  {
    template <typename T>
      requires std::is_floating_point_v<T> && std::numeric_limits<T>::is_iec559
    static unsigned Multiply(const auto &, const uint32_t *, uint32_t &, uint32_t &, uint32_t &) noexcept;

    template <>
    unsigned Multiply<float>(const uint32_t &mantissa, const uint32_t *table, uint32_t &first_9_digits, uint32_t &middle_9_digits, uint32_t &last_9_digits) noexcept
    {
      const constexpr uint32_t DEC8 = 100'000'000U;
      const constexpr uint32_t DEC9 = 1'000'000'000U;

      const uint64_t u64_prod_0 = static_cast<uint64_t>(mantissa) * table[0];
      const uint64_t u64_prod_1 = static_cast<uint64_t>(mantissa) * table[1];
      const uint64_t u64_prod_2 = static_cast<uint64_t>(mantissa) * table[2];

      const auto u32_0_prod_low = static_cast<uint32_t>(u64_prod_0);
      const auto u32_1_prod_low = static_cast<uint32_t>(u64_prod_1);
      const auto u32_1_prod_hig = static_cast<uint32_t>(u64_prod_1 >> 32U);
      const auto u32_2_prod_hig = static_cast<uint32_t>(u64_prod_2 >> 32U);
      const uint32_t u32_0_prod_low_1e9 = (uint64_t)u32_0_prod_low * DEC9 >> 32U;
      const uint32_t u32_1_prod_low_1e9 = (uint64_t)u32_1_prod_low * DEC9 >> 32U;

      first_9_digits = u64_prod_0 >> 32U;
      middle_9_digits = u32_1_prod_hig + u32_0_prod_low_1e9;
      last_9_digits = u32_2_prod_hig + u32_1_prod_low_1e9;

      const unsigned fir9 = ((first_9_digits < DEC8)) + ((~((first_9_digits >= DEC9) & 0b1U)) & 0b1U);
      const unsigned mid9 = ((middle_9_digits < DEC8)) + ((~((middle_9_digits >= DEC9) & 0b1U)) & 0b1U);
      const unsigned las9 = ((last_9_digits < DEC8)) + ((~((last_9_digits >= DEC9) & 0b1U)) & 0b1U);

      const unsigned status = fir9 | mid9 << 8U | las9 << 16U;

      return status;
    }

    template <>
    unsigned Multiply<float>(const uint64_t &mantissa, const uint32_t *table, uint32_t &first_9_digits, uint32_t &middle_9_digits, uint32_t &last_9_digits) noexcept
    {
      return 0U;
    }
  } // namespace Fixed
} // namespace Helpers::Math::IEEE754

//
