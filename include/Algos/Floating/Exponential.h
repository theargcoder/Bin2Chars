#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Helpers/Math.h"

#include "include/Constants/Constants.h"

#include "include/Algos/Integer.h"

#include "include/Helpers/Templating.h"

namespace Helpers::Numeric::Floating::ExponentialNotation
{
  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrCharArray(char *__restrict__ buff, const T &input, int PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10);

  template <>
  unsigned ToStrCharArray(char *__restrict__ buff, const float &input, int PRECISION)
  {
    using Floating = Constants::Tables::Floating<double>;

    const constexpr uint32_t ROUNDING_FACTOR = 5U;
    const constexpr uint32_t DEC9 = 1'000'000'000U;

    const constexpr auto PRECISION_TABLE = Constants::Tables::Exponential::GetPrecistionTable<unsigned>();

    assert(PRECISION <= 8); // no point of printing more than 8 or 17 digits respectively its actually not even necesary for round tripping

    unsigned len = 0;

    if(input < 0.0)
    {
      buff[len++] = '-';
    }

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
        len = 5;
        std::memcpy(&buff[0], "0.0E0", 5);
      }

      return len;
    }

    const auto *table = &Floating::DIGITS[exp_base_10_int][0];
    exp_base_10_int = (((exp_base_10_int - Floating::BIAS) * 78'913) >> 18U);

    unsigned first_9_digits, middle_9_digits, last_9_digits, remainder;
    const unsigned mul_cmp_res = Helpers::Simd::x86_64::Multiply<float>(mantissa, table, first_9_digits, middle_9_digits, last_9_digits);

    int lvl_1 = (1 - (mul_cmp_res & 0b11U));
    int lvl_2 = (1 - ((mul_cmp_res & 0b11'0000'0000U) >> 8U));
    int lvl_3 = (1 - ((mul_cmp_res & 0b11'0000'0000'0000'0000U) >> 16U));

    int round_lvl_1 = 8 + lvl_1;
    int round_lvl_2 = 8 + lvl_2;
    int round_lvl_3 = 8 + lvl_3;

    buff[len++] = '.';

    if(PRECISION < round_lvl_1)
    {
      const auto this_precision = PRECISION;
      const auto to_trunc = std::numeric_limits<unsigned>::digits10 - this_precision - 2 + lvl_1;
      exp_base_10_int += lvl_1;

      Helpers::Math::Precision::truncate_plus_1_quo_rem(first_9_digits, remainder, to_trunc);

      const bool extra = remainder != 0 || middle_9_digits != 0 || last_9_digits != 0;

      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(first_9_digits, remainder);

      if(remainder > ROUNDING_FACTOR)
      {
        first_9_digits++;
      }
      else if(remainder == ROUNDING_FACTOR)
      {
        if(extra)
        {
          first_9_digits++;
        }
        else
        {
          // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
          remainder = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<1>(first_9_digits); // digits_10 % 10;
          if(remainder & 1U)
          {
            first_9_digits++;
          }
        }
      }

      const auto precision_val = PRECISION_TABLE[PRECISION];
      if(first_9_digits >= precision_val)
      {
        Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(first_9_digits);
        exp_base_10_int++;
      }

      remainder = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len], first_9_digits);
    }
    else if(PRECISION < (round_lvl_1 + round_lvl_2))
    {
      const auto this_precision = PRECISION - round_lvl_1 - round_lvl_2;
      const auto to_trunc = std::numeric_limits<unsigned>::digits10 - this_precision - 2;
      exp_base_10_int += lvl_2;

      Helpers::Math::Precision::truncate_plus_1_quo_rem(middle_9_digits, remainder, to_trunc);

      const bool extra = remainder != 0 || last_9_digits != 0;

      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(middle_9_digits, remainder);

      if(remainder > ROUNDING_FACTOR)
      {
        middle_9_digits++;
      }
      else if(remainder == ROUNDING_FACTOR)
      {
        if(extra)
        {
          middle_9_digits++;
        }
        else
        {
          // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
          remainder = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<1>(middle_9_digits); // digits_10 % 10;
          if(remainder & 1U)
          {
            middle_9_digits++;
          }
        }
      }

      const auto precision_val = PRECISION_TABLE[this_precision];
      if(middle_9_digits >= precision_val)
      {
        while(middle_9_digits > precision_val)
        {
          first_9_digits++;
          middle_9_digits -= precision_val;
        }

        if(first_9_digits >= DEC9)
        {
          Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(middle_9_digits);
          exp_base_10_int++;
        }
      }

      remainder = 0;
      remainder += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len], first_9_digits);
      remainder += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len + remainder], middle_9_digits);
    }
    else if(PRECISION < (round_lvl_1 + round_lvl_2 + round_lvl_3))
    {
      const auto this_precision = PRECISION - round_lvl_1 - round_lvl_2;
      const auto to_trunc = std::numeric_limits<unsigned>::digits10 - this_precision - 2;
      exp_base_10_int += lvl_3;

      Helpers::Math::Precision::truncate_plus_1_quo_rem(middle_9_digits, remainder, to_trunc);

      const bool extra = remainder != 0;

      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(middle_9_digits, remainder);

      if(remainder > ROUNDING_FACTOR)
      {
        last_9_digits++;
      }
      else if(remainder == ROUNDING_FACTOR)
      {
        if(extra)
        {
          last_9_digits++;
        }
        else
        {
          // Apply Round-Ties-To-Even on the LAST VISIBLE DIGIT.
          remainder = Helpers::Math::Magic::Modulo::mod_by_10_pow_n<1>(middle_9_digits); // digits_10 % 10;
          if(remainder & 1U)
          {
            last_9_digits++;
          }
        }
      }

      const auto precision_val = PRECISION_TABLE[this_precision];
      if(last_9_digits >= precision_val)
      {
        while(last_9_digits > precision_val)
        {
          middle_9_digits++;
          last_9_digits -= precision_val;
        }

        if(middle_9_digits >= DEC9)
        {
          first_9_digits++;
          middle_9_digits -= DEC9;

          if(first_9_digits >= DEC9)
          {
            Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(middle_9_digits);
            exp_base_10_int++;
          }
        }
      }

      remainder = 0;
      remainder += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len], first_9_digits);
      remainder += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len + remainder], middle_9_digits);
      remainder += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<unsigned>(&buff[len + remainder], last_9_digits);
    } // else no rounding lol

    std::swap(buff[len - 1], buff[len]);

    len += remainder;

    buff[len++] = 'e';

    buff[len++] = (exp_base_10_int < 0) ? '-' : '+';

    const unsigned exp_abs = std::abs(exp_base_10_int);

    len += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<uint16_t>(&buff[len], static_cast<uint16_t>(exp_abs));

    return len;
  }

  template <>
  unsigned ToStrCharArray(char *__restrict__ buff, const double &input, int PRECISION)
  {
    using Floating = Constants::Tables::Floating<double>;
    using type = uint64_t;

    const constexpr type BASE = 10U;
    const constexpr type DEC8 = 100'000'000U;
    const constexpr uint32_t ROUNDING_FACTOR = 5U;
    const constexpr int32_t MAX_PRECISION = 17;

    const constexpr auto min_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - 2);
    const constexpr auto max_precision = Helpers::Math::Constexpr::ipow(type{ 10 }, std::numeric_limits<type>::digits10 - 1);

    const constexpr auto precision_table = Constants::Tables::Exponential::GetPrecistionTable<type>();

    assert(PRECISION <= MAX_PRECISION); // no point of printing more than 8 or 17 digits respectively its actually not even necesary for round tripping

    unsigned len = 0;

    if(input < 0.0)
    {
      buff[len++] = '-';
    }

    uint64_t mantissa;
    int exp_base_10_int;
    if(Helpers::Math::IEEE754::GetMantissaExponent<double>(input, mantissa, exp_base_10_int)) [[unlikely]]
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
        len = 5;
        std::memcpy(&buff[0], "0.0E0", 5);
      }

      return len;
    }

    const auto *table = &Floating::DIGITS[exp_base_10_int][0];
    exp_base_10_int = (((exp_base_10_int - Floating::BIAS) * 78'913) >> 18U);

    uint32_t extra_digits;
    type digits_10, remainder;
    Helpers::Math::IEEE754::Exponential::Multiply<double>(mantissa, table, digits_10, extra_digits);

    if(digits_10 < min_precision)
    {
      digits_10 *= BASE;
      remainder = Helpers::Math::Magic::Division::div_by_10_pow_n<8>(extra_digits);
      digits_10 += remainder;
      extra_digits -= remainder * DEC8;
      extra_digits *= BASE;
      exp_base_10_int--;
    }
    else if(digits_10 > max_precision)
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);
      Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(extra_digits);
      extra_digits += remainder * DEC8;
      exp_base_10_int++;
    }

    if(PRECISION < MAX_PRECISION)
    {
      Helpers::Math::Precision::truncate_plus_1_quo_rem(digits_10, remainder, std::numeric_limits<type>::digits10 - PRECISION - 3);
    }
    else
    {
      remainder = Helpers::Math::Magic::Division::div_by_10_pow_n<8>(extra_digits);
      extra_digits -= remainder * DEC8;
    }

    const bool extra = extra_digits != 0 || (PRECISION < 17 && remainder != 0);

    if(PRECISION < MAX_PRECISION)
    {
      Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<1>(digits_10, remainder);
    }

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

    if(digits_10 >= precision_table[PRECISION])
    {
      Helpers::Math::Magic::Division::div_by_10_pow_n_void<1>(digits_10);
      exp_base_10_int++;
    }

    buff[len++] = '.';

    remainder = Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<type>(&buff[len], digits_10);

    std::swap(buff[len - 1], buff[len]);

    len += remainder;

    buff[len++] = 'e';

    buff[len++] = (exp_base_10_int < 0) ? '-' : '+';

    const unsigned exp_abs = std::abs(exp_base_10_int);

    len += Helpers::Numeric::Integral::ToStrFowardWriteSIMDReturnLen<uint16_t>(&buff[len], static_cast<uint16_t>(exp_abs));

    return len;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Constants::Tables::Floating<T>::MAX_DIGITS10)
  {
    char buff[64];

    const uint32_t len = ToStrCharArray<T>(&buff[0], input, PRECISION);

    return std::string{ &buff[0], len };
  }
} // namespace Helpers::Numeric::Floating::ExponentialNotation
