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

      const constexpr unsigned BASE = 10U;
      const constexpr unsigned DEC7 = 10'000'000U;
      const constexpr unsigned DEC8 = 100'000'000U;
      const constexpr unsigned ROUNDING_FACTOR = 5U;

      const constexpr uint64_t MAGIC_10E8 = 1'441'151'881U;
      const constexpr uint8_t MAGIC_SHFT = 57U;

      const constexpr auto MIN_PRECISION = Helpers::Math::Constexpr::ipow(10U, std::numeric_limits<unsigned>::digits10 - 1);
      const constexpr auto MAX_PRECISION = Helpers::Math::Constexpr::ipow(10U, std::numeric_limits<unsigned>::digits10);

      const constexpr auto PRECISION_TABLE = Bin2Chars::Constants::Tables::Fixed::GetPrecistionTable<unsigned>();

      std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> EXP_DIGITS;

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
      buff[len++] = '.';

      exp_base_10_int -= Floating::BIAS;

      if(exp_base_10_int < 0)
      {
        Algos::Compute::DecimalExpansion::NegativeExponent(EXP_DIGITS, std::abs(exp_base_10_int));
      }
      else
      {
        Algos::Compute::DecimalExpansion::PositiveExponent(EXP_DIGITS, exp_base_10_int);
      }

      auto *it = &EXP_DIGITS[Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE - 1];

      while(*it == 0) // 2^0 is 1 so no way its 0 infinetly
      {
        it--;
      }

      for(unsigned carry = 0, digs, rem, carr;; it--)
      {
        const uint64_t prod = static_cast<uint64_t>(*it) * mantissa;
        digs = static_cast<unsigned>(prod >> 32U) + carry;
        carr = static_cast<unsigned>(prod);

        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(digs, rem);

        buff[len - 1] += digs;

        carry = Helpers::Assembly::umulh32(carr, DEC8);
        len += Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], rem);

        if(it == &EXP_DIGITS[0])
        {
          len += Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], carry);
          break;
        }
      }

      return len = 0;
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
} // namespace Helpers::Numeric::Floating::DigitsPrecision
