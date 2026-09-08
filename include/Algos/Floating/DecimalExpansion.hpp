#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

#include "include/Algos/Integer.hpp"
#include "include/Constants/Constants.hpp"
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
    static unsigned ToStr(char *__restrict__ /*buff*/, const float & /*input*/, int /*PRECISION*/)
    {
      return 0U;
    }
  };

  template <>
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::TRUNCATE, double>
  {
    static unsigned ToStr(char *__restrict__ /*buff*/, const double & /*input*/, int /*PRECISION*/)
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

      Helpers::Assembly::prefetch_elements<1026>(Bin2Chars::Tables::PositiveExponent::INDICES);
      Helpers::Assembly::prefetch_elements<1076>(Bin2Chars::Tables::NegativeExponent::INDICES);

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
          std::memset(&buff[len], '0', static_cast<size_t>(PRECISION));
          len += static_cast<unsigned>(PRECISION);
        }

        return len;
      }

      exp -= Floating::BIAS;

      const uint32_t *it_beg = (exp < 0) ? &Bin2Chars::Tables::NegativeExponent::TABLE[Bin2Chars::Tables::NegativeExponent::INDICES[std::abs(exp)]]
                                         : &Bin2Chars::Tables::PositiveExponent::TABLE[Bin2Chars::Tables::PositiveExponent::INDICES[exp]];

      const uint32_t *it_end = (exp < 0) ? &Bin2Chars::Tables::NegativeExponent::TABLE[0] + Bin2Chars::Tables::NegativeExponent::INDICES[std::abs(exp) + 1]
                                         : &Bin2Chars::Tables::PositiveExponent::TABLE[0] + Bin2Chars::Tables::PositiveExponent::INDICES[exp + 1];
      const uint32_t *it = it_end - 1;

      (exp < 0) ? Helpers::Assembly::prefetch_elements<96>(&Bin2Chars::Tables::NegativeExponent::TABLE[Bin2Chars::Tables::NegativeExponent::INDICES[std::abs(exp)]])
                : Helpers::Assembly::prefetch_elements<39>(&Bin2Chars::Tables::PositiveExponent::TABLE[Bin2Chars::Tables::PositiveExponent::INDICES[exp]]);

      unsigned start_idx = 0;
      if(input < 0.0F)
      {
        buff[len++] = '-';
        start_idx = 1;
      }

      const int expected_digits = static_cast<int>(Helpers::Simd::calculate_len(*(it_end - 1)));
      const int n_limbs = static_cast<int>(it_end - it_beg - 1);

      int exp_base_10 = expected_digits - 1 + (n_limbs << 3U) - (exp < 0 ? std::abs(exp) : 0);

      uint64_t prod = static_cast<uint64_t>(*it) * mantissa;
      auto digs = static_cast<unsigned>(prod >> 32U);
      uint32_t frac = static_cast<unsigned>(prod);

      const int actual_digits = (digs == 0) ? 1 : static_cast<int>(Helpers::Simd::calculate_len(digs));
      exp_base_10 -= (expected_digits - actual_digits);

      int precision_missing;
      unsigned rem, len_written, int_len;

      if(exp_base_10 < 0)
      {
        int_len = 1;
        precision_missing = 1 + PRECISION;
        const auto exp_base_10_ABS = std::abs(exp_base_10);
        const auto n_zeros = static_cast<unsigned>(std::min(exp_base_10_ABS, precision_missing));
        std::memset(&buff[len], '0', n_zeros);
        precision_missing -= static_cast<int>(n_zeros);
        len += n_zeros;

        if(exp_base_10_ABS - 1 > PRECISION)
        {
          buff[len++] = '0';
          buff[(input < 0.0F) ? 2 : 1] = '.';
          return len;
        }
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
        prod = static_cast<uint64_t>(*it) * mantissa;

        const uint64_t total = static_cast<uint64_t>(frac) * DEC8 + prod;

        digs = static_cast<unsigned>(total >> 32U);
        frac = static_cast<unsigned>(total);

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

        len_written = Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], rem);
        len += len_written;
        precision_missing -= static_cast<int>(len_written);
      }

      while(precision_missing >= 0 && frac != 0) // write all digits and change since they are needed for rounding
      {
        const uint64_t step_total = static_cast<uint64_t>(frac) * DEC8;
        digs = static_cast<unsigned>(step_total >> 32U);
        frac = static_cast<unsigned>(step_total);

        len_written = Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], digs);
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

  template <>
  struct ToStrWriteBuffReturnLenImpl<Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND, double>
  {
    static unsigned ToStr(char *__restrict__ buff, const double &input, int PRECISION)
    {
      using uint128_t = __uint128_t;
      using Floating = Bin2Chars::Constants::Tables::Floating<double>;

      const constexpr unsigned DEC8 = 100'000'000U;

      Helpers::Assembly::prefetch_elements<1026>(Bin2Chars::Tables::PositiveExponent::INDICES);
      Helpers::Assembly::prefetch_elements<1076>(Bin2Chars::Tables::NegativeExponent::INDICES);

      unsigned len = 0;
      uint64_t mantissa;
      int exp;
      if(Helpers::Math::IEEE754::GetMantissaExponent<double>(input, mantissa, exp)) [[unlikely]]
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

      exp -= Floating::BIAS;

      const uint32_t *it_beg = (exp < 0) ? &Bin2Chars::Tables::NegativeExponent::TABLE[Bin2Chars::Tables::NegativeExponent::INDICES[std::abs(exp)]]
                                         : &Bin2Chars::Tables::PositiveExponent::TABLE[Bin2Chars::Tables::PositiveExponent::INDICES[exp]];

      const uint32_t *it_end = (exp < 0) ? &Bin2Chars::Tables::NegativeExponent::TABLE[0] + Bin2Chars::Tables::NegativeExponent::INDICES[std::abs(exp) + 1]
                                         : &Bin2Chars::Tables::PositiveExponent::TABLE[0] + Bin2Chars::Tables::PositiveExponent::INDICES[exp + 1];
      const uint32_t *it = it_end - 1;

      (exp < 0) ? Helpers::Assembly::prefetch_elements<96>(&Bin2Chars::Tables::NegativeExponent::TABLE[Bin2Chars::Tables::NegativeExponent::INDICES[std::abs(exp)]])
                : Helpers::Assembly::prefetch_elements<39>(&Bin2Chars::Tables::PositiveExponent::TABLE[Bin2Chars::Tables::PositiveExponent::INDICES[exp]]);

      unsigned start_idx = 0;
      if(input < 0.0)
      {
        buff[len++] = '-';
        start_idx = 1;
      }

      const int expected_digits = static_cast<int>(Helpers::Simd::calculate_len(*(it_end - 1)));
      const int n_limbs = static_cast<int>(it_end - it_beg - 1);

      int exp_base_10 = expected_digits - 1 + (n_limbs << 3U) - (exp < 0 ? std::abs(exp) : 0);

      uint128_t prod = static_cast<uint128_t>(*it) * mantissa;
      auto digs = static_cast<uint32_t>(prod >> 64U);
      auto frac = static_cast<uint64_t>(prod);

      const int actual_digits = (digs == 0) ? 1 : static_cast<int>(Helpers::Simd::calculate_len(digs));
      exp_base_10 -= (expected_digits - actual_digits);

      int precision_missing;
      unsigned rem, len_written, int_len;

      if(exp_base_10 < 0)
      {
        int_len = 1;
        precision_missing = 1 + PRECISION;
        const auto exp_base_10_ABS = std::abs(exp_base_10);
        const auto n_zeros = static_cast<unsigned>(std::min(exp_base_10_ABS, precision_missing));
        std::memset(&buff[len], '0', n_zeros);
        precision_missing -= static_cast<int>(n_zeros);
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
        int_len = static_cast<unsigned>(exp_base_10) + 1;
        precision_missing = static_cast<int>(int_len) + PRECISION;
      }

      len_written = Helpers::Simd::x86_64::WriteCharsToPtrFowardReturnLength<unsigned>(&buff[len], digs);
      precision_missing -= static_cast<int>(len_written);
      len += len_written;
      it--;

      for(; it >= it_beg && precision_missing >= -8; it--) // 8 extra chars (1 chungk) should suffice for rounding purposes ... right??
      {
        prod = static_cast<uint128_t>(*it) * mantissa;

        const auto total = static_cast<uint128_t>(frac) * DEC8 + prod;

        digs = static_cast<unsigned>(total >> 64U);
        frac = static_cast<uint64_t>(total);

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

        len_written = Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], rem);
        len += len_written;
        precision_missing -= static_cast<int>(len_written);
      }

      while(precision_missing >= 0 && frac != 0) // write all digits and change since they are needed for rounding
      {
        const uint128_t step_total = static_cast<uint128_t>(frac) * DEC8;
        digs = static_cast<unsigned>(step_total >> 64U);
        frac = static_cast<uint64_t>(step_total);

        len_written = Helpers::Simd::x86_64::WriteEightCharsToPtrFowardReturnLength<unsigned>(&buff[len], digs);
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
