#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Algos/Integer.h"

#include "include/Helpers/Simd.h"
#include "include/Helpers/Templating.h"

namespace Helpers::Numeric::Floating::DigitsPrecision
{
  enum class RoundingBehavior : uint8_t
  {
    TRUNCATE,
    ROUND,
    CEIL,
    FLOOR
  };

  template <RoundingBehavior BEHAVE, typename T>
  struct ToStrWriteBuffReturnLenImpl;

  template <Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior BEHAVE, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrWriteBuffReturnLen(char *__restrict__ ptr, const T &input, int PRECISION)
  {
    return Helpers::Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLenImpl<BEHAVE, T>::ToStr(ptr, input, PRECISION);
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct ToStrWriteBuffReturnLenImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, T>
  {
    static unsigned ToStr(char *__restrict__ buff, const T &input, int PRECISION);
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, float>
  {
    static unsigned ToStr(char *__restrict__ buff, const float &input, int PRECISION)
    {
      return 0U;
    }
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, double>
  {
    static unsigned ToStr(char *__restrict__ buff, const double &input, int PRECISION)
    {
      return 0U;
    }
  };

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct ToStrWriteBuffReturnLenImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, T>
  {
    static unsigned ToStr(char *__restrict__ buff, const T &input, int PRECISION);
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, float>
  {
    static unsigned ToStr(char *__restrict__ buff, const float &input, int PRECISION)
    {
      using Floating = Constants::Tables::Floating<double>;

      const constexpr unsigned BASE = 10U;
      const constexpr unsigned DEC8 = 100'000'000U;
      const constexpr unsigned ROUNDING_FACTOR = 5U;

      const constexpr auto MIN_PRECISION = Helpers::Math::Constexpr::ipow(10U, std::numeric_limits<unsigned>::digits10 - 1);
      const constexpr auto MAX_PRECISION = Helpers::Math::Constexpr::ipow(10U, std::numeric_limits<unsigned>::digits10);

      const constexpr auto PRECISION_TABLE = Constants::Tables::Fixed::GetPrecistionTable<unsigned>();

      unsigned len = 0;
      unsigned mantissa;
      int exp_base_10_int;
      if(Helpers::Math::IEEE754::GetMantissaExponent<float>(input, mantissa, exp_base_10_int)) [[unlikely]]
      {
        if(mantissa == 0)
        {
          len = 3;
          std::memcpy(&buff[0], "nan", 3);
        }
        else if(mantissa == 1)
        {
          len = 3;
          std::memcpy(&buff[0], "inf", 3);
        }
        else if(mantissa == 2)
        {
          len = 4;
          std::memcpy(&buff[0], "-inf", 4);
        }
        else
        {
          len = 2;
          std::memcpy(&buff[0], "0.", len);
          std::memset(&buff[len], '0', PRECISION);
          len += PRECISION;
        }

        return len;
      }

      if(input < 0.0)
      {
        buff[len++] = '-';
      }

      const auto *table = &Floating::DIGITS[exp_base_10_int][0];
      exp_base_10_int = (((exp_base_10_int - Floating::BIAS) * 78'913) >> 18U);

      unsigned remainder;
      unsigned digits_10, extra_digits, last_9_digits;
      const auto res_simd_mul = Helpers::Simd::x86_64::Multiply<float>(mantissa, table, digits_10, extra_digits, last_9_digits);
      const auto res_iee_mul = Helpers::Math::IEEE754::Fixed::Multiply<float>(mantissa, table, digits_10, extra_digits, last_9_digits);

      if(res_simd_mul != res_iee_mul)
      {
        const auto res_simd_mul_1 = Helpers::Simd::x86_64::Multiply<float>(mantissa, table, digits_10, extra_digits, last_9_digits);
        const auto res_iee_mul_1 = Helpers::Math::IEEE754::Fixed::Multiply<float>(mantissa, table, digits_10, extra_digits, last_9_digits);
        assert(res_iee_mul == res_simd_mul);
      }

      if(digits_10 < MIN_PRECISION)
      {
        digits_10 *= BASE;
        remainder = Helpers::Math::Magic::Division::div_by_10_pow_n<8>(extra_digits);
        digits_10 += remainder;
        extra_digits -= remainder * DEC8;
        extra_digits *= BASE;
        exp_base_10_int--;
      }
      else if(digits_10 > MAX_PRECISION)
      {
        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);
        Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(extra_digits);
        extra_digits += remainder * DEC8;
        exp_base_10_int++;
      }

      unsigned exp_10_abs = std::abs(exp_base_10_int + 1), visible_digits;

      if(exp_base_10_int < 0)
      {
        const int to_comp = PRECISION + exp_base_10_int + 1;
        visible_digits = std::max(to_comp, 0);
      }
      else
      {
        const int to_comp = PRECISION + exp_base_10_int + 1;
        visible_digits = std::min(to_comp, std::numeric_limits<unsigned>::digits10 - 1);
      }

      if(visible_digits >= 0 && visible_digits <= std::numeric_limits<unsigned>::digits10 - 2)
      {
        const auto trunc_qty = std::numeric_limits<unsigned>::digits10 - visible_digits - 1;
        Helpers::Math::Precision::truncate_plus_1_quo_rem(digits_10, remainder, trunc_qty);

        const bool extra = extra_digits != 0 || remainder != 0;

        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);

        if(remainder > ROUNDING_FACTOR)
        {
          digits_10++;
        }
        else if(remainder == ROUNDING_FACTOR)
        {
          if(extra)
          {
            digits_10++;
          }
          else
          {
            // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
            remainder = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<1>(digits_10); // digits_10 % 10;
            if(remainder & 1U)
            {
              digits_10++;
            }
          }
        }

        const auto presicion = PRECISION_TABLE[visible_digits];

        if(digits_10 >= presicion)
        {
          // Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(digits_10);
          exp_base_10_int++;
        }
      }

      exp_10_abs = std::abs(exp_base_10_int + 1);

      if(exp_base_10_int < 0)
      {
        buff[len++] = '0';
        buff[len++] = '.';
        if(exp_10_abs > PRECISION)
        {
          std::memset(&buff[len], '0', PRECISION);
          len += PRECISION;
        }
        else
        {
          std::memset(&buff[len], '0', exp_10_abs);
          len += exp_10_abs;
          Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen(&buff[len], digits_10);
          len += PRECISION - exp_10_abs;
        }
      }
      else
      {
        if(exp_base_10_int <= std::numeric_limits<unsigned>::digits10 - 2)
        {
          const auto len_wr = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen(&buff[len], digits_10);
          std::memmove(&buff[len + exp_10_abs + 1U], &buff[len + exp_10_abs], len_wr);
          buff[len + exp_10_abs] = '.';
          len += exp_10_abs;
          len += PRECISION;
        }
        else
        {
          const auto len_wr_1 = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen(&buff[len], digits_10);
          const auto len_wr_2 = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen(&buff[len + len_wr_1], digits_10);
          const auto missin = std::max(0, static_cast<int>(exp_10_abs - len_wr_1 - len_wr_2));
          std::memset(&buff[len + len_wr_1 + len_wr_2], '0', missin);
          std::memmove(&buff[len + exp_10_abs + 1U], &buff[len + exp_10_abs], len_wr_1 + len_wr_2);
          buff[len + exp_10_abs] = '.';
          len += exp_10_abs + 1;
          std::memset(&buff[len], '0', PRECISION);
          len += PRECISION;
        }
      }

      return len;
    }
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, double>
  {
    static unsigned ToStr(char *__restrict__ buff, const double &input, int PRECISION)
    {
      return 0U;
    }
  };

  template <RoundingBehavior BEHAVE, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int PRECISION)
  {
    char buff[2048]; // massive on purpose
    const unsigned len = Helpers::Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLen<BEHAVE, T>(&buff[0], input, PRECISION);
    return std::string{ &buff[0], len };
  }
} // namespace Helpers::Numeric::Floating::DigitsPrecision
