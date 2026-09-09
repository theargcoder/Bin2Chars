#pragma once

#include <sys/cdefs.h>
#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

#include "include/Helpers/Math.hpp"
#include "include/Helpers/Simd.hpp"
#include "include/Helpers/Templating.hpp"

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace Bin2Chars::Numeric::Integral
{
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
  static inline std::string ToStr(const T &input) noexcept
  {
    constexpr size_t size = (sizeof(T) == 1) ? 4 : (sizeof(T) == 2) ? 8 : (sizeof(T) <= 4) ? 11 : 20;

    std::string buff;

    buff.resize_and_overwrite(size,
                              [&input](char *__restrict__ ptr, size_t /*unused*/) noexcept
                              {
                                const bool neg = input < 0;
                                using UT = Helpers::Templating::Types::make_unsigned_t<T>;

                                const UT val = neg ? static_cast<UT>(~static_cast<UT>(input) + UT{ 1 }) : static_cast<UT>(input);

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
  static inline std::string ToStr(const T &input) noexcept
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

} // namespace Bin2Chars::Numeric::Integral

//
///
