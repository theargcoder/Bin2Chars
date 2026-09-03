#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

#include "include/Algos/Compute/DecimalExpansion.h"
#include "include/Algos/Integer.h"
#include "include/Constants/Constants.h"
#include "include/Helpers/Assembly.h"
#include "include/Helpers/Math.h"
#include "include/Helpers/Simd.h"
#include "include/Helpers/Templating.h"

namespace Bin2Chars::Numeric::Floating::DigitsPrecision
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

  template <Numeric::Floating::DigitsPrecision::RoundingBehavior BEHAVE, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrWriteBuffReturnLen(char *__restrict__ ptr, const T &input, int PRECISION)
  {
    return Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLenImpl<BEHAVE, T>::ToStr(ptr, input, PRECISION);
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, T>
  {
    static unsigned ToStr(char *__restrict__ buff, const T &input, int PRECISION);
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, float>
  {
    static unsigned ToStr(char *__restrict__ buff, const float &input, int PRECISION)
    {
      return 0U;
    }
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, double>
  {
    static unsigned ToStr(char *__restrict__ buff, const double &input, int PRECISION)
    {
      return 0U;
    }
  };

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, T>
  {
    static unsigned ToStr(char *__restrict__ buff, const T &input, int PRECISION);
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, float>
  {
    static unsigned ToStr(char *__restrict__ buff, const float &input, int PRECISION)
    {
      using Floating = Bin2Chars::Constants::Tables::Floating<double>;

      const constexpr unsigned DEC8 = 100'000'000U;
      const constexpr unsigned ROUNDING_FACTOR = 5U;

      const constexpr uint64_t MAGIC_10E8 = 1'441'151'881U;
      const constexpr uint8_t MAGIC_SHFT = 57U;

      std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> EXP_DIGITS;

      unsigned len = 0;
      unsigned mantissa;
      int exp;
      if(Helpers::Math::IEEE754::GetMantissaExponent<float>(input, mantissa, exp)) [[unlikely]]
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
      buff[len++] = '.';

      exp -= Floating::BIAS;

      unsigned carry = 0, digs, rem, carr;
      auto *it = &EXP_DIGITS[Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE - 1];

      if(int exp_base_10; exp < 0)
      {
        Algos::Compute::DecimalExpansion::NegativeExponent(EXP_DIGITS, std::abs(exp));

        while(*it == 0) // 2^0 is 1 so no way its 0 infinetly
        {
          it--;
        }
        const uint64_t prod = static_cast<uint64_t>(*it) * mantissa;
        digs = static_cast<unsigned>(prod >> 32U) + carry;
        carr = static_cast<unsigned>(prod);

        const unsigned digits = Helpers::Simd::calculate_len(digs);

        const int n_limbs = it - &EXP_DIGITS[0];

        exp_base_10 = static_cast<int>(digits) - 1 + (8 * n_limbs) - std::abs(exp);

        const auto exp_base_10_ABS = std::abs(exp_base_10);

        std::memset(&buff[len], '0', exp_base_10_ABS);
        std::swap(buff[len - 1], buff[len]);
        len += exp_base_10_ABS;

        if(exp_base_10_ABS > PRECISION + 1) // if there are more leading zeros than precision available ...
        {
          return 1 + PRECISION + 1;
        }

        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(digs, rem);
        carry = Helpers::Assembly::umulh32(carr, DEC8);

        buff[len - 1] += digs;
        len += Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<unsigned>(&buff[len], rem);
        it--;
      }
      else
      {
        Algos::Compute::DecimalExpansion::PositiveExponent(EXP_DIGITS, exp);
      }

      for(; it >= &EXP_DIGITS[0] && len < PRECISION + 9; it--)
      {
        const uint64_t prod = static_cast<uint64_t>(*it) * mantissa;
        digs = static_cast<unsigned>(prod >> 32U) + carry;
        carr = static_cast<unsigned>(prod);

        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(digs, rem);
        carry = Helpers::Assembly::umulh32(carr, DEC8);

        buff[len - 1] += digs;
        len += Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], rem);
      }

      len += Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], carry);

      if(size_t i = 1 + PRECISION + 1; buff[i] > '5')
      {
        buff[1 + PRECISION]++;
      }
      else if(buff[i] == '5')
      {
        i++;
        for(; i < len && buff[i] == '0'; i++)
        {
        }
        if(i != len)
        {
          buff[1 + PRECISION]++;
        }
      }

      return 1 + PRECISION + 1;
    }
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, double>
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
    const unsigned len = Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLen<BEHAVE, T>(&buff[0], input, PRECISION);
    return std::string{ &buff[0], len };
  }
} // namespace Bin2Chars::Numeric::Floating::DigitsPrecision
