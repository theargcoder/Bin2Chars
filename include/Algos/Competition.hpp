#pragma once

#include <ryu/ryu.h>

#include <array>
#include <charconv>
#include <cstddef>
#include <string>
#include <system_error>
#include <type_traits>

namespace Bin2Chars::Numeric::Std
{
  template <bool SCIENTIFIC, typename Type>
  static auto to_string(Type value, const int &PRECISION)
  {
    std::string buff;

    if constexpr(std::is_floating_point_v<Type>)
    {
      if constexpr(SCIENTIFIC)
      {
        const auto size = static_cast<size_t>(PRECISION) + 30;
        buff.resize_and_overwrite(size,
                                  [&value, PRECISION](char *__restrict__ ptr, size_t in_size)
                                  {
                                    const auto [res_ptr, ec] = std::to_chars(ptr, ptr + in_size, value, std::chars_format::scientific, PRECISION);
                                    return res_ptr - ptr;
                                  });
        return buff;
      }
      else
      {
        const auto size = static_cast<size_t>(PRECISION) + 340;
        buff.resize_and_overwrite(size,
                                  [&value, PRECISION](char *__restrict__ ptr, const size_t in_size)
                                  {
                                    const auto [res_ptr, ec] = std::to_chars(ptr, ptr + in_size, value, std::chars_format::fixed, PRECISION);
                                    return res_ptr - ptr;
                                  });
        return buff;
      }
    }
    else if constexpr(std::is_integral_v<Type>)
    {
      constexpr size_t size = (sizeof(Type) == 1) ? 5 : (sizeof(Type) == 2) ? 8 : (sizeof(Type) <= 4) ? 11 : 20;

      buff.resize_and_overwrite(size,
                                [&value](char *__restrict__ ptr, const size_t in_size)
                                {
                                  const auto [res_ptr, ec] = std::to_chars(ptr, ptr + in_size, value);
                                  return res_ptr - ptr;
                                });
      return buff;
    }
  }

  // Generic implementation for arbitrary bases.
  template <typename Tp>
  unsigned to_chars_len(Tp value) noexcept
  {
    unsigned n = 1;
    const unsigned b2 = 100;
    const unsigned b3 = b2 * 10;
    const unsigned long b4 = b3 * 10;

    for(;;)
    {
      if(value < 10U)
        return n;
      if(value < b2)
        return n + 1;
      if(value < b3)
        return n + 2;
      if(value < b4)
        return n + 3;
      value /= static_cast<Tp>(b4);
      n += 4;
    }
  }

  // Write an unsigned integer value to the range [first,first+len). The caller is required to provide a buffer of exactly the right size (which can be determined by the
  // __to_chars_len function).
  template <typename Tp>
  auto to_chars_impl(char *first, Tp val) noexcept
  {
    constexpr char digits[201] = "0001020304050607080910111213141516171819"
                                 "2021222324252627282930313233343536373839"
                                 "4041424344454647484950515253545556575859"
                                 "6061626364656667686970717273747576777879"
                                 "8081828384858687888990919293949596979899";

    const unsigned len = to_chars_len(val);
    unsigned pos = len - 1;
    while(val >= 100)
    {
      auto const num = (val % 100) * 2;
      val /= 100;
      first[pos] = digits[num + 1];
      first[pos - 1] = digits[num];
      pos -= 2;
    }
    if(val >= 10)
    {
      auto const num = val * 2;
      first[1] = digits[num + 1];
      first[0] = digits[num];
    }
    else
      first[0] = static_cast<char>('0' + val);

    return len;
  }

} // namespace Bin2Chars::Numeric::Std

namespace Bin2Chars::Numeric::Ryu
{
  namespace Exponential
  {
    static std::string ToStr(double v, const int &PRECISION)
    {
      const auto size = static_cast<size_t>(PRECISION) + 30;

      std::string buff;

      buff.resize_and_overwrite(size, [&v, &PRECISION](char *__restrict__ ptr, size_t /*unused*/) noexcept { return d2exp_buffered_n(v, static_cast<unsigned>(PRECISION), ptr); });

      return buff;
    }

    static std::string ToStr(float v, const int &PRECISION)
    {
      return ToStr(static_cast<double>(v), PRECISION);
    }
  } // namespace Exponential

  namespace Fixed
  {
    static std::string ToStr(double v, const int &PRECISION)
    {
      const auto size = static_cast<size_t>(PRECISION) + 340;

      std::string buff;

      buff.resize_and_overwrite(size,
                                [&v, &PRECISION](char *__restrict__ ptr, size_t /*unused*/) noexcept { return d2fixed_buffered_n(v, static_cast<unsigned>(PRECISION), ptr); });

      return buff;
    }

    static std::string ToStr(float v, const int &PRECISION)
    {
      return ToStr(static_cast<double>(v), PRECISION);
    }
  } // namespace Fixed
} // namespace Bin2Chars::Numeric::Ryu
