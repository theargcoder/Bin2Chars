#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

#include "include/Algos/Compute/DecimalExpansion.hpp"
#include "include/Algos/Integer.hpp"
#include "include/Constants/Constants.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Math.hpp"
#include "include/Helpers/Simd.hpp"
#include "include/Helpers/Templating.hpp"

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

      unsigned start_idx = 0;
      if(input < 0.0)
      {
        buff[len++] = '-';
        start_idx = 1;
      }

      exp -= Floating::BIAS;

      auto *it = &EXP_DIGITS[Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE - 1];

      if(exp < 0)
      {
        Algos::Compute::DecimalExpansion::NegativeExponent(EXP_DIGITS, std::abs(exp));
      }
      else
      {
        Algos::Compute::DecimalExpansion::PositiveExponent(EXP_DIGITS, exp);
      }

      while(*it == 0) // 2^0 is 1 so no way its 0 infinetly
      {
        it--;
      }

      int exp_base_10, precision_missing;
      unsigned rem, len_written, int_len;

      uint64_t prod = static_cast<uint64_t>(*it) * mantissa;
      auto digs = static_cast<unsigned>(prod >> 32U);
      uint32_t frac = static_cast<unsigned>(prod); // Renamed from carr. carry is GONE!

      const auto digits = static_cast<int>(Helpers::Simd::calculate_len(digs));
      const int n_limbs = static_cast<int>(it - &EXP_DIGITS[0]);

      exp_base_10 = digits - 1 + (n_limbs << 3U) - (exp < 0 ? std::abs(exp) : 0);

      if(exp_base_10 < 0)
      {
        int_len = 1;
        precision_missing = 1 + PRECISION;
        const auto exp_base_10_ABS = std::abs(exp_base_10);
        const auto n_zeros = static_cast<unsigned>(std::min(exp_base_10_ABS, precision_missing));

        std::memset(&buff[len], '0', n_zeros);
        precision_missing -= n_zeros;
        len += n_zeros;

        if(exp_base_10_ABS - 1 > PRECISION)
        {
          buff[len++] = '0';
          buff[(input < 0.0) ? 2 : 1] = '.';
          return len;
        }
      }
      else
      {
        int_len = exp_base_10 + 1;
        precision_missing = int_len + PRECISION;
      }

      len_written = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<unsigned>(&buff[len], digs);
      precision_missing -= static_cast<int>(len_written);
      len += len_written;
      it--;

      for(; it >= &EXP_DIGITS[0] && precision_missing > 0; it--)
      {
        prod = static_cast<uint64_t>(*it) * mantissa;

        const uint64_t total = static_cast<uint64_t>(frac) * DEC8 + prod;

        digs = static_cast<unsigned>(total >> 32U);
        frac = static_cast<unsigned>(total);

        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(digs, rem);

        // buff[len - 1] += digs; // FUXK this no longer stands since buff[len -1]  can be '9' ....

        if(digs != 0)
        {
          int i = static_cast<int>(len) - 1;
          const int ST = static_cast<int>(start_idx);
          for(; i >= ST; i--)
          {
            if(buff[i] == '9')
            {
              buff[i] = '0';
            }
            else
            {
              buff[i]++;
              break;
            }
          }

          if(i < ST) // rippled all the way to hell
          {
            std::memmove(&buff[ST + 1], &buff[ST], len - start_idx); // move the shit down 1 slot
            buff[start_idx] = '1';                                   // add the leading zero
            len++;
            exp_base_10++;
            int_len++;
          }
        }

        len_written = Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], rem);
        len += len_written;
        precision_missing -= static_cast<int>(len_written);
      }

      // Final trailing fraction conversion (replacing your final WriteEightChars carry)
      const uint64_t final_total = static_cast<uint64_t>(frac) * DEC8;
      digs = static_cast<unsigned>(final_total >> 32U);
      frac = static_cast<unsigned>(final_total); // Kept perfectly exact in case your rounder wants to inspect it

      len_written = Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], digs);
      len += len_written;
      precision_missing -= static_cast<int>(len_written);

      if(precision_missing > 0)
      {
        std::memset(&buff[len], '0', precision_missing);
        len += static_cast<int>(precision_missing);
      }

      if(PRECISION > 0)
      {
        const unsigned dot_idx = start_idx + int_len;

        std::memmove(&buff[dot_idx + 1], &buff[dot_idx], len - dot_idx);

        buff[dot_idx] = '.';
        len++;
      }

      // Calculate original target string length (truncating the extra generation)
      len = static_cast<unsigned>(static_cast<int>(len) + precision_missing);
      const size_t MAX = len + std::abs(precision_missing);

      bool round_up = false;
      if(MAX > len)
      {
        const char next_digit = buff[len];
        if(next_digit > '5')
        {
          round_up = true;
        }
        else if(next_digit == '5')
        {
          bool trailing_zeros = true;

          // 1. Check the already-generated extra digits in the buffer
          for(size_t j = len + 1; j < MAX; j++)
          {
            if(buff[j] != '0')
            {
              trailing_zeros = false;
              break;
            }
          }

          // 2. Check the unprocessed Bignum limbs
          // If the generation loop broke early, 'it' points to the first unprocessed limb.
          // If any remaining limb is non-zero, the mathematical remainder is > 0.
          if(trailing_zeros && it >= &EXP_DIGITS[0])
          {
            for(auto *rem_it = it; rem_it >= &EXP_DIGITS[0]; rem_it--)
            {
              if(*rem_it != 0)
              {
                trailing_zeros = false;
                break;
              }
            }
          }

          if(trailing_zeros)
          {
            // Banker's Rounding: Exact tie, round to even.
            // (len - 1 is guaranteed to be a digit, not '.', because we only insert '.'
            // if PRECISION > 0, meaning there is at least one digit after it)
            round_up = ((buff[len - 1] - '0') & 1U);
          }
          else
          {
            // Strictly greater than 0.5
            round_up = true;
          }
        }
      }

      if(round_up)
      {
        int i = static_cast<int>(len) - 1;
        const int ST = static_cast<int>(start_idx);
        for(; i >= ST; i--)
        {
          if(buff[i] == '.')
          {
            continue;
          }

          if(buff[i] == '9')
          {
            buff[i] = '0';
          }
          else
          {
            buff[i]++;
            break;
          }
        }

        // Ripple carry propagated past the most significant digit (e.g., 99.9 -> 100.0)
        if(i < ST)
        {
          // Shift the valid string right by 1 byte.
          // MAGIC: This shifts the '.' right by 1 position as well, perfectly preserving PRECISION!
          std::memmove(&buff[ST + 1], &buff[ST], len - start_idx);
          buff[start_idx] = '1';
          len++;
        }
      }

      if(unsigned i; exp_base_10 > 0 && buff[start_idx] == '0') // CANT HAVE LEADING ZEROS
      {
        for(i = start_idx; i < len && buff[i] == '0'; i++)
        {
        }
        std::memmove(&buff[start_idx], &buff[i], len - i);
        len -= i - start_idx;
      }

      return len;
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
