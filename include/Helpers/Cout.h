
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace Helpers::Cout
{

  template <typename T>
  void print_binary(const std::string &label, const T &input)
  {
    using underlying = std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
    using signed_underlying = std::conditional_t<std::is_same_v<T, float>, int32_t, int64_t>;

    static constexpr auto BITS = 8;
    static constexpr signed_underlying IS_DOUBLE = std::is_same_v<T, double>;
    static constexpr underlying EXPONENT_ST = IS_DOUBLE ? 52 : 23;
    static constexpr signed_underlying EXP_LEFT_OFFSET = sizeof(T) * 8 - EXPONENT_ST - 1;

    underlying in_in_print = std::bit_cast<underlying>(input);

    std::cout << label << "\t= ";
    int i = sizeof(T) * BITS - 1;
    std::cout << ((((underlying(1) << i) & in_in_print) == 0) ? 0 : 1);
    std::cout << " | ";
    i--;
    for(int j = 0; j < EXP_LEFT_OFFSET - 1; j++)
    {
      std::cout << ((((underlying(1) << i) & in_in_print) == 0) ? 0 : 1);
      i--;
    }
    std::cout << " | ";
    while(i >= 0)
    {
      std::cout << ((((underlying(1) << i) & in_in_print) == 0) ? 0 : 1);
      i--;
    }
    std::cout << "\n";
  };
} // namespace Helpers::Cout
