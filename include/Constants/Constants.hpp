#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

#include "include/Helpers/Math.hpp"

namespace Bin2Chars::Constants
{
  struct Size
  {
    const constexpr static char MAX_FMT_SIZE = 8;
  };

  struct Interactive
  {
    const constexpr static bool debug_blink = true;
  };

  struct Delimiters
  {
    const constexpr static char open = '{';
    const constexpr static char close = '}';
  };

  struct Colors
  {
    const constexpr static uint8_t bool_value_true = 214;
    const constexpr static uint8_t bool_value_false = 214;
    const constexpr static uint8_t nullptrs = 196;
    const constexpr static uint8_t pointers = 237;
    const constexpr static uint8_t chars = 8;
    const constexpr static uint8_t ints = 130;
    const constexpr static uint8_t floats = ints;
    const constexpr static uint8_t doubles = ints;
    const constexpr static uint8_t strings = 76;
    const constexpr static uint8_t unknown = 12;
  };

  struct Ansi
  {
    const constexpr static char *begin = "\033[";
    const constexpr static char *none = "0;";
    const constexpr static char *italic = "3;";
    const constexpr static char *bold = "1;";
    const constexpr static char *blink = "5;";
    const constexpr static char *st_color = "38;5;";
    const constexpr static char *en_color = "m";
    const constexpr static char *reset = "\033[0m";
  };
} // namespace Bin2Chars::Constants

namespace Bin2Chars::Constants::Tables
{
  template <typename T>
    requires std::is_floating_point_v<T>
  struct Floating
  {
  public:
    static const constexpr auto MANTISSA_BITS = std::numeric_limits<double>::digits - 1;
    static const constexpr int EXPONENT_BIAS = std::numeric_limits<double>::max_exponent - 1;
    static const constexpr int MIN_BIN_EXP = std::numeric_limits<double>::min_exponent - std::numeric_limits<double>::digits; // Smallest binary exponent (subnormal limit)
    static const constexpr auto MAX_BIN_EXP = std::numeric_limits<double>::max_exponent;                                      // Largest binary exponent
    static const constexpr auto BIAS = -MIN_BIN_EXP;                                                                          // Offset so that table[BIAS] corresponds to 2^0
    static const constexpr auto TABLE_BIAS = std::is_same_v<double, T> ? 0 : BIAS + std::numeric_limits<float>::min_exponent; // Offset so that float's have correct locations

    static const constexpr auto SIZE = MAX_BIN_EXP - MIN_BIN_EXP + 1;
    static const constexpr auto MAX_DIGITS10 = std::numeric_limits<T>::digits10;
    static const constexpr auto MAX_EXP_DIGITS10
        = static_cast<std::remove_cvref_t<decltype(MIN_BIN_EXP)>>(Bin2Chars::Helpers::Math::Constexpr::log10(T{ std::numeric_limits<T>::max_exponent10 }));
  };
} // namespace Bin2Chars::Constants::Tables

//
