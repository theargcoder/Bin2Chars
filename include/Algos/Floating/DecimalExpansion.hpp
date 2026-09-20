#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <string>
#include <type_traits>

#include "include/Algos/Integer.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Math.hpp"
#include "include/Helpers/Simd.hpp"
#include "include/Helpers/Templating.hpp"

#include "include/Tables/array_2n.hpp"
#include "include/Tables/array_5n.hpp"

namespace Bin2Chars::Numeric::Floating::DigitsPrecision
{
  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrWriteBuffReturnLen(char *buff, const T &input, int PRECISION)
  {
    if(PRECISION < 0) // UB if negative precision
    {
      std::terminate();
    }

    using base_t = uint64_t;

    const constexpr unsigned DEC8 = 100'000'000U;

    unsigned len = 0;
    uint8_t sign;
    base_t mantissa;
    int exp;
    if(Helpers::Math::IEEE754::GetMantissaExponent<T, base_t>(input, sign, mantissa, exp)) [[unlikely]]
    {
      if(mantissa == 0)
      {
        std::memcpy(&buff[0], "nan", len = 3);
      }
      else if(mantissa == 1)
      {
        len = 3;
        std::memcpy(&buff[0], "inf", len = 3);
      }
      else if(mantissa == 2)
      {
        std::memcpy(&buff[0], "-inf", len = 4);
      }
      else
      {
        std::memset(&buff[0], '0', len = 1);
        if(PRECISION > 0)
        {
          std::memset(&buff[len++], '.', 1);
          std::memset(&buff[len], '0', static_cast<size_t>(PRECISION)), len += static_cast<unsigned>(PRECISION);
        }
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
    if(sign != 0)
    {
      buff[len++] = '-';
      start_idx++;
    }

    const int n_limbs = static_cast<int>(it_end - it_beg - 1);

    int exp_base_10 = (n_limbs << 3U) - (exp < 0 ? abs_exp : 0) - 1;

    uint32_t digs;
    base_t frac;
    Helpers::Assembly::umul96(static_cast<base_t>(*it), mantissa, frac, digs);

    const int actual_digits = static_cast<int>(Helpers::Simd::calculate_len(digs));
    exp_base_10 += actual_digits;

    int precision_missing;
    unsigned rem, len_written, int_len;

    if(exp_base_10 >= 0)
    {
      int_len = static_cast<unsigned>(exp_base_10) + 1;
      precision_missing = static_cast<int>(int_len) + PRECISION;

      if(int_len == 1 || digs != 0)
      {
        len_written = Bin2Chars::Numeric::Integral::ToStrBufferedReturnLen<unsigned>(&buff[len], digs);
        precision_missing -= static_cast<int>(len_written);
        len += len_written;
      }
      else
      {
        int_len--;
        precision_missing--;
      }

      it--;

      for(; it >= it_beg && precision_missing > PRECISION; it--)
      {
        base_t curr_prod_lo;
        uint32_t curr_prod_hi;
        Helpers::Assembly::umul96(static_cast<base_t>(*it), mantissa, curr_prod_lo, curr_prod_hi);

        base_t dec_lo;
        uint32_t dec_hi;
        Helpers::Assembly::umul64x32_96(frac, DEC8, dec_lo, dec_hi);

        const base_t total_lo = dec_lo + curr_prod_lo;
        const base_t carry = total_lo < dec_lo ? 1U : 0U;

        digs = dec_hi + curr_prod_hi + static_cast<uint32_t>(carry);
        frac = total_lo;

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

        Bin2Chars::Numeric::Integral::ToStrBufferedNumChars<8>(&buff[len], rem);
        len += 8;
        precision_missing -= 8;
      }

      if(PRECISION > 0)
      {
        std::memmove(&buff[start_idx + int_len + 1], &buff[start_idx + int_len], 8);

        buff[start_idx + int_len] = '.';
        len++;
      }
    }
    else
    {
      if(PRECISION <= 0)
      {
        buff[len++] = '0';
        return len;
      }
      int_len = 1;
      buff[len++] = '.';

      precision_missing = 1 + PRECISION;

      const auto exp_base_10_ABS = static_cast<unsigned>(std::abs(exp_base_10));

      if(exp_base_10_ABS > static_cast<unsigned>(precision_missing + 3))
      {
        std::memset(&buff[len], '0', static_cast<size_t>(precision_missing));

        std::swap(buff[start_idx], buff[start_idx + 1]);
        return len + static_cast<unsigned>(precision_missing);
      }

      std::memset(&buff[len], '0', exp_base_10_ABS);
      precision_missing -= static_cast<int>(exp_base_10_ABS);
      len += exp_base_10_ABS;

      len_written = Bin2Chars::Numeric::Integral::ToStrBufferedReturnLen<unsigned>(&buff[len], digs);
      precision_missing -= static_cast<int>(len_written);
      len += len_written;
      it--;

      std::swap(buff[start_idx], buff[start_idx + 1]);
    }

    for(; it >= it_beg && precision_missing >= -8; it--) // 8 extra chars (1 chungk) should suffice for rounding purposes ... right??
    {
      base_t curr_prod_lo;
      uint32_t curr_prod_hi;
      Helpers::Assembly::umul96(static_cast<base_t>(*it), mantissa, curr_prod_lo, curr_prod_hi);

      base_t dec_lo;
      uint32_t dec_hi;
      Helpers::Assembly::umul64x32_96(frac, DEC8, dec_lo, dec_hi);

      const base_t total_lo = dec_lo + curr_prod_lo;
      const base_t carry = total_lo < dec_lo ? 1U : 0U;

      digs = dec_hi + curr_prod_hi + static_cast<uint32_t>(carry);
      frac = total_lo;

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

      Bin2Chars::Numeric::Integral::ToStrBufferedNumChars<8>(&buff[len], rem);
      len += 8;
      precision_missing -= 8;
    }

    while(precision_missing >= 0 && frac != 0) // write all digits and change since they are needed for rounding
    {
      Helpers::Assembly::umul64x32_96(frac, DEC8, frac, digs);

      Bin2Chars::Numeric::Integral::ToStrBufferedNumChars<8>(&buff[len], digs);
      len += 8;
      precision_missing -= 8;
    }

    if(precision_missing > 0)
    {
      std::memset(&buff[len], '0', static_cast<size_t>(precision_missing));
      len += static_cast<unsigned>(precision_missing);
      precision_missing = 0; // CRITICAL: Prevent double-adding length at the end!
    }

    const size_t MAX = len;
    len = static_cast<unsigned>(static_cast<int>(len) + precision_missing);

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

        if(trailing_zeros && (frac != 0 || it >= it_beg))
        {
          trailing_zeros = false;
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

    return len;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int PRECISION)
  {
    const auto size = static_cast<size_t>(PRECISION) + 340;

    std::string buff;

    buff.resize_and_overwrite(size, [&input, &PRECISION](char *ptr, size_t /*unused*/) noexcept { return ToStrWriteBuffReturnLen<T>(ptr, input, PRECISION); });

    return buff;
  }
} // namespace Bin2Chars::Numeric::Floating::DigitsPrecision
