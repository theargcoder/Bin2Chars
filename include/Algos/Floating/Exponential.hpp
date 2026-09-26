#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <type_traits>
#include <utility>

#include "include/Algos/Compute/ExponentDecimalExpansion.hpp"
#include "include/Algos/Floating/DecimalExpansion.hpp"
#include "include/Algos/Integer.hpp"
#include "include/Helpers/Assembly.hpp"
#include "include/Helpers/Math.hpp"
#include "include/Helpers/Templating.hpp"

namespace Bin2Chars::Numeric::Floating::ExponentialNotation
{
  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static unsigned ToStrCharArray(char *buff, const T &input, int PRECISION = Algos::Compute::DecimalExpansion::Traits<T>::MAX_DIGITS10)
  {
    if(PRECISION < 0) [[unlikely]] // UB if negative precision
    {
      std::terminate();
    }

    using base_t = uint64_t;

    constexpr unsigned DEC9 = 1'000'000'000U;

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
        std::memcpy(&buff[0], "inf", len = 3);
      }
      else if(mantissa == 2)
      {
        std::memcpy(&buff[0], "-inf", len = 4);
      }
      else
      {
        buff[0] = '0', len = 1;
        if(PRECISION > 0)
        {
          buff[len++] = '.';
          std::memset(&buff[len], '0', static_cast<size_t>(PRECISION)), len += static_cast<unsigned>(PRECISION);
        }
        std::memcpy(&buff[len], "e+00", 4), len += 4;
      }

      return len;
    }

    const int abs_exp = std::abs(exp);
    const bool is_pos = exp >= 0;

    const uint16_t *idx_ptr = (is_pos) ? Bin2Chars::Tables::PositiveExponent::INDICES : Bin2Chars::Tables::NegativeExponent::INDICES;
    const uint32_t *table_ptr = (is_pos) ? Bin2Chars::Tables::PositiveExponent::TABLE : Bin2Chars::Tables::NegativeExponent::TABLE;
    const uint32_t idx_beg = *(idx_ptr + abs_exp);
    const uint32_t idx_end = *(idx_ptr + abs_exp + 1);

    const uint32_t *it_beg = table_ptr + idx_beg;
    const uint32_t *it_end = table_ptr + idx_end;
    const uint32_t *it = it_end - 1;

    unsigned start_idx = sign;
    buff[len] = '-';
    len += sign;

    buff[len] = '.';
    len += PRECISION > 0;
    start_idx += PRECISION > 0;

    const int n_limbs = static_cast<int>(it_end - it_beg - 1);

    int exp_base_10 = (n_limbs * 9) - (exp < 0 ? abs_exp : 0) - 1;

    uint32_t digs;
    base_t frac;
    Helpers::Assembly::umul96(static_cast<base_t>(*it), mantissa, frac, digs);

    int precision_missing = 1 + PRECISION; // always leading digit in exponential formatting

    // no leading zero; always non zero number . xxx; adjust exponent to correct
    unsigned len_written = (digs != 0) ? Bin2Chars::Numeric::Integral::ToStrBufferedReturnLen<unsigned>(&buff[len], digs) : 0;
    precision_missing -= static_cast<int>(len_written);
    len += len_written;
    exp_base_10 += static_cast<int>(len_written);

    it--;

    for(; it >= it_beg && precision_missing >= -9; it--) // 9 extra chars (1 chungk) should suffice for rounding purposes ... right??
    {
      base_t curr_prod_lo;
      uint32_t curr_prod_hi;
      Helpers::Assembly::umul96(static_cast<base_t>(*it), mantissa, curr_prod_lo, curr_prod_hi);

      base_t dec_lo;
      uint32_t dec_hi;
      Helpers::Assembly::umul64x32_96(frac, DEC9, dec_lo, dec_hi);

      const base_t total_lo = dec_lo + curr_prod_lo;
      const base_t carry = total_lo < dec_lo ? 1U : 0U;

      digs = dec_hi + curr_prod_hi + static_cast<uint32_t>(carry);
      frac = total_lo;

      if(digs > 999'999'999) [[unlikely]]
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
          buff[len++] = '0';     // one digit more
          buff[start_idx] = '1'; // add the leading 1
          exp_base_10++;
          precision_missing--;
        }
      }

      Bin2Chars::Numeric::Integral::ToStrBufferedExactlyNumChars<9>(&buff[len], digs);
      len += 9;
      precision_missing -= 9;
    }

    while(precision_missing >= 0 && frac != 0) // write all digits and change since they are needed for rounding
    {
      Helpers::Assembly::umul64x32_96(frac, DEC9, frac, digs);

      Bin2Chars::Numeric::Integral::ToStrBufferedExactlyNumChars<9>(&buff[len], digs);
      len += 9;
      precision_missing -= 9;
    }

    if(precision_missing > 0)
    {
      std::memset(&buff[len], '0', static_cast<size_t>(precision_missing));
      len += static_cast<unsigned>(precision_missing);
      precision_missing = 0; // CRITICAL: Prevent double-adding length at the end!
    }

    const size_t MAX = len;
    len = static_cast<unsigned>(static_cast<int>(len) + precision_missing);

    if(MAX > len)
    {
      bool round_up = false;
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

      if(round_up)
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

        if(i < ST)
        {
          buff[start_idx] = '1';
          exp_base_10++; // round up all the way to hell increases exponent
        }
      }
    }

    if(PRECISION > 0)
    {
      std::swap(buff[start_idx - 1], buff[start_idx]);
    }

    buff[len++] = 'e';
    buff[len++] = (exp_base_10 < 0) ? '-' : '+';

    // minimum of 2 digits for exp; if there is 3 then 3 digits
    len += Bin2Chars::Numeric::Integral::ToStrBufferedAtLeastNumChars<2>(&buff[len], static_cast<uint16_t>(std::abs(exp_base_10)));

    buff[len] = '\0';

    return len;
  }

  template <typename T>
    requires std::is_floating_point_v<T> && (Helpers::Templating::Assert::at_most_64_bit_double_radix_2<T>())
  static std::string ToStr(const T &input, const int &PRECISION = Algos::Compute::DecimalExpansion::Traits<T>::MAX_DIGITS10)
  {
    const auto size = static_cast<size_t>(PRECISION) + 30;

    std::string buff;

    buff.resize_and_overwrite(size, [&input, &PRECISION](char *ptr, size_t /*unused*/) noexcept { return ToStrCharArray<T>(ptr, input, PRECISION); });

    return buff;
  }
} // namespace Bin2Chars::Numeric::Floating::ExponentialNotation
