#pragma once

#include <sys/cdefs.h>
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

#include "include/Helpers/Math.h"
#include "include/Helpers/Simd.h"
#include "include/Helpers/Templating.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace Helpers::Numeric::Integral
{
  template <int N>
  struct char_array_len
  {
    int length;
    char array[N];
  };

  template <int N>
  struct char_array
  {
    int start_idx;
    char array[N];
  };

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static auto ToStrCharArray(const T &input)
  {
    static const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10 + 2;

    char_array<MAX_DIGITS10> buff;
    buff.start_idx = MAX_DIGITS10;

    char *__restrict__ it = &buff.array[buff.start_idx];

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;

    const bool NEGATIVE = input < 0;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);
    static const constexpr UT BASE = UT{ 10 };

    do
    {
      const auto rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val);

    if(NEGATIVE)
    {
      *--it = '-';
    }
    else if constexpr(FORCE_SIGN)
    {
      *--it = '+';
    }

    buff.start_idx = it - &buff.array[0];

    return buff;
  }

  template <bool FORCE_SIGN = false, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static std::string ToStr(const T &input)
  {
    const auto buff = Helpers::Numeric::Integral::ToStrCharArray<FORCE_SIGN>(input);
    return std::string(&buff.array[buff.start_idx], sizeof(buff.array) - buff.start_idx);
  }

  template <typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>) || std::is_same_v<T, __uint128_t>
  static uint32_t ToStrFowardWriteSIMDReturnLen(char *__restrict__ buff, const T &input)
  {
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
    return Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<T>(buff, input);
#elif defined(__ARM_NEON) || defined(__aarch64__)
    return Helpers::Simd::ARM64::WriteCharsToPtrFowardReturnLength<T>(buff, input);
#endif
  }

  template <typename T>
    requires std::is_integral_v<T> && std::is_signed_v<T>
  static inline std::string ToStrSIMD(const T &input) noexcept
  {
    constexpr size_t size = (sizeof(T) == 1) ? 4 : (sizeof(T) == 2) ? 8 : (sizeof(T) <= 4) ? 11 : 20;

    std::string buff;

    buff.resize_and_overwrite(size,
                              [&input](char *__restrict__ ptr, size_t /*unused*/) noexcept
                              {
                                const bool neg = input < 0;
                                using UT = Helpers::Templating::Types::make_unsigned_t<T>;
                                UT val = (neg) ? ~(static_cast<UT>(input)) + 1U : input;

                                *ptr = '-';

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
                                const auto len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<UT>(ptr + static_cast<unsigned>(neg), val) + static_cast<unsigned>(neg);
#elif defined(__ARM_NEON) || defined(__aarch64__)
                                const auto len = Helpers::Simd::ARM64::WriteCharsToPtrFowardReturnLength<UT>(ptr + static_cast<unsigned>(neg), val) + static_cast<unsigned>(neg);
#endif

                                return len;
                              });
    return buff;
  }

  template <typename T>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
  static inline std::string ToStrSIMD(const T &input) noexcept
  {
    constexpr size_t size = (sizeof(T) == 1) ? 4 : (sizeof(T) == 2) ? 8 : (sizeof(T) <= 4) ? 10 : 20;

    std::string buff;

    buff.resize_and_overwrite(size,
                              [&input](char *__restrict__ ptr, size_t /*unused*/) noexcept
                              {
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
                                const uint32_t len = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<T>(ptr, input);
#elif defined(__ARM_NEON) || defined(__aarch64__)
                                const uint32_t len = Helpers::Simd::ARM64::WriteCharsToPtrFowardReturnLength<T>(ptr, input);
#endif
                                return len;
                              });

    return buff;
  }

  template <int N, typename T>
    requires(std::is_integral_v<T> && std::is_unsigned_v<T>) || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArrayResult(T &val, T &rem, char_array<N> &out_char)
  {
    char *__restrict__ it = &out_char.array[0] + out_char.start_idx;

    do
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(val, rem);

      *--it = '0' + rem;

    } while(val);

    out_char.start_idx = it - &out_char.array[0];
  }

  template <bool FORCE_SIGN = false, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArray(const T &input, char_array<N> &out_char, const int &st_idx)
  {
    char *__restrict__ it = &out_char.array[st_idx];

    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

    do
    {
      const auto rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val);

    if(NEGATIVE)
    {
      *--it = '-';
    }
    else if constexpr(FORCE_SIGN)
    {
      *--it = '+';
    }

    out_char.start_idx = it - &out_char.array[0];
  }

  template <uint32_t CAP_FORCE_LENGTH, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static void ToStrReverseWriteToCharArrayForceAndCapLength(const T &input, char_array<N> &out_char, const int &st_idx)
  {
    static_assert(N > CAP_FORCE_LENGTH, "cant force more chars than number of chars that can fit in the buffer bruh");

    char *__restrict__ it = &out_char.array[st_idx];

    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);

    uint32_t i = 0;
    do
    {
      i++;
      const auto rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val && i < CAP_FORCE_LENGTH);

    const auto len = CAP_FORCE_LENGTH - i;

    it -= len;

    std::memset(it, '0', len);

    if(NEGATIVE)
    {
      *--it = '-';
    }

    out_char.start_idx = it - &out_char.array[0];
  }

  template <uint32_t CAP_LENGTH, int N, typename T>
    requires std::is_integral_v<T> || std::is_same_v<T, __uint128_t>
  static auto ToStrReverseWriteToCharArrayCapLengthStopAtNthCharReturnRemainder(const T &input, char_array<N> &out_char, const uint32_t &st_idx, const uint32_t stp_idx)
  {
    static const constexpr auto BASE = 10;
    const bool NEGATIVE = input < 0;

    char *__restrict__ it = &out_char.array[st_idx];

    using UT = Helpers::Templating::Types::make_unsigned_t<T>;
    UT val = NEGATIVE ? static_cast<UT>(-(input + 1)) + 1 : static_cast<UT>(input);
    UT rem;

    uint32_t i = 0;
    do
    {
      i++;
      rem = val % BASE;
      val /= BASE;

      *--it = '0' + rem;

    } while(val && i < stp_idx && i < CAP_LENGTH);

    if(NEGATIVE)
    {
      *--it = '-';
    }

    out_char.start_idx = it - &out_char.array[0];

    return (val) ? val % BASE : 0;
  }
} // namespace Helpers::Numeric::Integral

//
///
