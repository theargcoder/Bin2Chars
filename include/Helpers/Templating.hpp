#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

namespace Bin2Chars::Helpers::Templating::Types
{
  template <typename T>
  static constexpr bool is_char_v()
  {
    using BaseType = std::remove_cvref_t<T>;
    return std::is_same_v<BaseType, char>;
  }

  template <typename T>
  static constexpr bool is_numeric_v()
  {
    using BaseType = std::remove_cvref_t<T>;
    return (std::is_integral_v<BaseType> && !std::is_same_v<BaseType, bool>) || std::is_floating_point_v<BaseType>;
  }

  template <typename T>
  static constexpr bool is_not_numeric_v()
  {
    return !is_numeric_v<T>();
  }

  template <typename T>
  static constexpr bool is_string_v()
  {
    using BaseType = std::remove_cvref_t<T>;
    return std::is_same_v<BaseType, char *> || std::is_same_v<BaseType, const char *> || std::is_same_v<BaseType, std::string>;
  }

  template <typename T>
  static constexpr bool is_bool_v()
  {
    using BaseType = std::remove_cvref_t<T>;
    return std::is_same_v<BaseType, bool>;
  }

  template <typename T>
  static constexpr bool is_at_most_64_bit_float_radix_2()
  {
    return sizeof(T) <= 8 && std::numeric_limits<T>::radix == 2;
  }

  template <typename T>
  struct make_unsigned
  {
    using type = std::make_unsigned_t<T>;
  };

  template <>
  struct make_unsigned<__int128>
  {
    using type = __uint128_t;
  };

  template <>
  struct make_unsigned<__uint128_t>
  {
    using type = __uint128_t;
  };

  template <typename T>
  using make_unsigned_t = typename make_unsigned<T>::type;

} // namespace Bin2Chars::Helpers::Templating::Types

namespace Bin2Chars::Helpers::Templating::Assert
{
  template <typename T>
  static constexpr bool at_most_64_bit_double_radix_2()
  {
    static_assert(Helpers::Templating::Types::is_at_most_64_bit_float_radix_2<T>(),
                  "Type is larger than 64 bit or has radix is not base 2; check your double implementation in your systems docs");
    return true;
  }
} // namespace Bin2Chars::Helpers::Templating::Assert

namespace Bin2Chars::Helpers::Templating::Fillers
{
  template <size_t M, size_t P, typename... Chars>
    requires(std::is_same_v<char, Chars> && ...)
  static consteval void fill_char_array(char (&array)[M][P], auto &idx, const Chars &...args)
  {
    size_t a = 0;
    ((array[idx][a++] = args), ...);
  }

  template <size_t A, size_t X, size_t Y, typename... Args>
    requires(std::is_integral_v<Args> && ...)
  static void fill_decrement_char_array(char (&dest)[A], const char (&src)[X][Y], uint32_t &dest_idx, const auto &src_idx, const Args &...args)
  {
    ((dest[dest_idx--] = src[src_idx][args]), ...);
  }
} // namespace Bin2Chars::Helpers::Templating::Fillers
