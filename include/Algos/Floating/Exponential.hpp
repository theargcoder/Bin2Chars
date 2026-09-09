#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>

#include "include/Constants/Constants.hpp"
#include "include/Helpers/Templating.hpp"

namespace Bin2Chars::Numeric::Floating::ExponentialNotation
{
  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrCharArray(char *__restrict__ buff, const T &input, int PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10);

  template <>
  unsigned ToStrCharArray(char *__restrict__ /*buff*/, const float & /*input*/, int /*PRECISION*/)
  {
    return 0;
  }

  template <>
  unsigned ToStrCharArray(char *__restrict__ /*buff*/, const double & /*input*/, int /*PRECISION*/)
  {
    return 0;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    char buff[64];

    const uint32_t len = ToStrCharArray<T>(&buff[0], input, PRECISION);

    return std::string{ &buff[0], len };
  }
} // namespace Bin2Chars::Numeric::Floating::ExponentialNotation
