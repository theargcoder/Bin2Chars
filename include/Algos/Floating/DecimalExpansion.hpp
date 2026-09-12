#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

#include "include/Algos/Compute/ExponentDecimalExpansion.hpp"
#include "include/Algos/Integer.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Math.hpp"
#include "include/Helpers/Simd.hpp"
#include "include/Helpers/Templating.hpp"

#include "include/Tables/array_2n.hpp"
#include "include/Tables/array_5n.hpp"

namespace Bin2Chars::Numeric::Floating::DigitsPrecision
{
  enum class RoundingBehavior : uint8_t
  {
    ROUND,
    CEIL,
    FLOOR
  };

  template <RoundingBehavior BEHAVE, typename T>
  struct ToStrWriteBuffReturnLenImpl
  {
  };

  template <Numeric::Floating::DigitsPrecision::RoundingBehavior BEHAVE, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrWriteBuffReturnLen(char *__restrict__ ptr, const T &input, int PRECISION)
  {
    return Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLenImpl<BEHAVE, T>::ToStr(ptr, input, PRECISION);
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, T>
  {
    static unsigned ToStr(char *__restrict__ buff, const T &input, int PRECISION)
    {
      using base_t = uint64_t;    // std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;
      using wide_t = __uint128_t; //  std::conditional_t<std::is_same_v<float, T>, uint64_t, __uint128_t>;

      constexpr auto SHIFT_T = 64U; // std::is_same_v<float, T> ? 32U : 64U;

      const constexpr unsigned DEC8 = 100'000'000U;

      unsigned len = 0;
      base_t mantissa;
      int exp;
      if(Helpers::Math::IEEE754::GetMantissaExponent<T, base_t>(input, mantissa, exp)) [[unlikely]]
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
          std::memset(&buff[len], '0', static_cast<size_t>(PRECISION));
          len += static_cast<unsigned>(PRECISION);
        }

        return len;
      }

      const int abs_exp = std::abs(exp);
      const int pos_exp = (exp >= 0) ? exp : 0;

      const uint32_t neg_idx_beg = Bin2Chars::Tables::NegativeExponent::INDICES[abs_exp];
      const uint32_t neg_idx_end = Bin2Chars::Tables::NegativeExponent::INDICES[abs_exp + 1];

      const uint32_t pos_idx_beg = Bin2Chars::Tables::PositiveExponent::INDICES[pos_exp];
      const uint32_t pos_idx_end = Bin2Chars::Tables::PositiveExponent::INDICES[pos_exp + 1];

      const uint32_t *it_beg = (exp < 0) ? &Bin2Chars::Tables::NegativeExponent::TABLE[neg_idx_beg] : &Bin2Chars::Tables::PositiveExponent::TABLE[pos_idx_beg];
      const uint32_t *it_end = (exp < 0) ? &Bin2Chars::Tables::NegativeExponent::TABLE[neg_idx_end] : &Bin2Chars::Tables::PositiveExponent::TABLE[pos_idx_end];

      const uint32_t *it = it_end - 1;

      // prefetch both to avoid branching and since it will only ocuppy ~7 cache lines its fine
      Helpers::Assembly::prefetch_elements<96>(&Bin2Chars::Tables::NegativeExponent::TABLE[neg_idx_beg]);
      Helpers::Assembly::prefetch_elements<39>(&Bin2Chars::Tables::PositiveExponent::TABLE[pos_idx_beg]);

      unsigned start_idx = 0;
      if(input < static_cast<T>(0.0))
      {
        buff[len++] = '-';
        start_idx = 1;
      }

      const int expected_digits = static_cast<int>(Helpers::Simd::calculate_len(*(it)));
      const int n_limbs = static_cast<int>(it_end - it_beg - 1);

      int exp_base_10 = expected_digits - 1 + (n_limbs << 3U) - (exp < 0 ? abs_exp : 0);

      const wide_t prod = static_cast<wide_t>(*it) * mantissa;
      unsigned digs = static_cast<unsigned>(prod >> SHIFT_T);
      base_t frac = static_cast<base_t>(prod);

      const int actual_digits = (digs == 0) ? 1 : static_cast<int>(Helpers::Simd::calculate_len(digs));
      exp_base_10 -= (expected_digits - actual_digits);

      int precision_missing;
      unsigned rem, len_written, int_len;

      if(exp_base_10 < 0)
      {
        int_len = 1;
        precision_missing = 1 + PRECISION;
        const auto exp_base_10_ABS = static_cast<unsigned>(std::abs(exp_base_10));
        std::memset(&buff[len], '0', exp_base_10_ABS);
        precision_missing -= static_cast<int>(exp_base_10_ABS);
        len += exp_base_10_ABS;
      }
      else
      {
        int_len = static_cast<unsigned>(exp_base_10) + 1;
        precision_missing = static_cast<int>(int_len) + PRECISION;
      }

      len_written = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<unsigned>(&buff[len], digs);
      precision_missing -= static_cast<int>(len_written);
      len += len_written;
      it--;

      for(; it >= it_beg && precision_missing >= -8; it--) // 8 extra chars (1 chungk) should suffice for rounding purposes ... right??
      {
        const wide_t prod = static_cast<wide_t>(*it) * mantissa;
        const wide_t total = static_cast<wide_t>(frac) * DEC8 + prod;

        digs = static_cast<unsigned>(total >> SHIFT_T);
        frac = static_cast<base_t>(total);

        Helpers::Math::Magic::Modulo::mod_by_10_pow_n_void<8>(digs, rem);

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
            std::memmove(&buff[ST + 1], &buff[ST], len - start_idx); // move the full expansion down 1 slot
            buff[start_idx] = '1';                                   // add the leading zero
            len++;
            exp_base_10++;
            int_len++;
          }
        }

        len_written = Helpers::Simd::x86_64::WriteNumCharsToPtrFowardReturnLength<8>(&buff[len], rem);
        len += len_written;
        precision_missing -= static_cast<int>(len_written);
      }

      while(precision_missing >= 0 && frac != 0) // write all digits and change since they are needed for rounding
      {
        const wide_t step_total = static_cast<wide_t>(frac) * DEC8;
        digs = static_cast<unsigned>(step_total >> SHIFT_T);
        frac = static_cast<base_t>(step_total);

        len_written = Helpers::Simd::x86_64::WriteNumCharsToPtrFowardReturnLength<8>(&buff[len], digs);
        len += len_written;
        precision_missing -= static_cast<int>(len_written);
      }

      if(precision_missing > 0)
      {
        std::memset(&buff[len], '0', static_cast<size_t>(precision_missing));
        len += static_cast<unsigned>(precision_missing);
        precision_missing = 0; // CRITICAL: Prevent double-adding length at the end!
      }

      if(PRECISION > 0)
      {
        const unsigned dot_idx = start_idx + int_len;

        std::memmove(&buff[dot_idx + 1], &buff[dot_idx], len - dot_idx);

        buff[dot_idx] = '.';
        len++;
      }

      len = static_cast<unsigned>(static_cast<int>(len) + precision_missing);
      const size_t MAX = len + static_cast<unsigned>(std::abs(precision_missing));

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

          for(size_t j = len + 1; j < MAX; j++)
          {
            if(buff[j] != '0')
            {
              trailing_zeros = false;
              break;
            }
          }

          if(trailing_zeros && frac != 0)
          {
            trailing_zeros = false;
          }

          if(trailing_zeros && it >= it_beg)
          {
            for(const auto *rem_it = it; rem_it >= it_beg; rem_it--)
            {
              if(*rem_it != 0)
              {
                trailing_zeros = false;
                break;
              }
            }
          }

          if(trailing_zeros) // bankers round
          {
            round_up = (static_cast<unsigned>(buff[len - 1] - '0') & 1U);
          }
          else // round
          {
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

        if(i < ST)
        {
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

  template <RoundingBehavior BEHAVE, typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int PRECISION)
  {
    char buff[2048]; // massive on purpose
    const unsigned len = Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLen<BEHAVE, T>(&buff[0], input, PRECISION);
    return std::string{ &buff[0], len };
  }
} // namespace Bin2Chars::Numeric::Floating::DigitsPrecision
