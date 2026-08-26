#define BOOST_TEST_MODULE compute_decimal_expansion

#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "include/Algos/Compute/DecimalExpansion.h"
#include "include/Constants/Constants.h"

namespace
{
  void compute_pos_exponent_safe(std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> &RESULT, const int &k)
  {
    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    RESULT.fill(0);
    RESULT[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 2 in each iteration)
    unsigned i = 0;
    for(i = 0; i + 5 < k; i += 5)
    {
      uint32_t carry = 0;
      for(unsigned int &w : RESULT)
      {
        uint64_t pp = (static_cast<uint64_t>(w) << 5U) + carry;
        carry = static_cast<uint32_t>((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = static_cast<uint32_t>(pp - static_cast<uint64_t>(carry * 100'000'000U));
      }
    }

    const unsigned miss = static_cast<unsigned>(k) - i;
    uint32_t carry = 0;
    for(unsigned int &w : RESULT)
    {
      uint64_t pp = (static_cast<uint64_t>(w) << miss) + carry;
      carry = static_cast<uint32_t>((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = static_cast<uint32_t>(pp - static_cast<uint64_t>(carry * 100'000'000U));
    }

    size_t first = 0;

    while(first < RESULT.size() && RESULT.at(first) == 0)
    {
      ++first;
    }

    if(first != 0 && first != RESULT.size())
    {
      const size_t count = RESULT.size() - first;

      std::memmove(RESULT.data(), RESULT.data() + first, count * sizeof(uint32_t));

      std::memset(RESULT.data() + count, 0, first * sizeof(uint32_t));
    }
  }

  void compute_neg_exponent_safe(std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> &RESULT, const int &k)
  {
    RESULT.fill(0);
    RESULT[0] = 1; // Initialize 5^0 = 1

    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    const auto pairs = static_cast<uint32_t>(k / 2); // 537 passes of x25
    for(uint32_t i = 0; i < pairs; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : RESULT)
      {
        uint64_t pp = static_cast<uint64_t>(w) * 25U + carry;
        carry = static_cast<uint32_t>((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = static_cast<uint32_t>(pp - static_cast<uint64_t>(carry * 100'000'000U));
      }
    }

    if(k % 2 != 0)
    {
      uint32_t carry = 0;
      for(unsigned int &w : RESULT)
      {
        uint64_t pp = static_cast<uint64_t>(w) * 5U + carry;
        carry = static_cast<uint32_t>((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = static_cast<uint32_t>(pp - static_cast<uint64_t>(carry * 100'000'000U));
      }
    }

    size_t first = 0;

    while(first < RESULT.size() && RESULT[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != RESULT.size())
    {
      const size_t count = RESULT.size() - first;

      std::memmove(RESULT.data(), RESULT.data() + first, count * sizeof(uint32_t));

      std::memset(RESULT.data() + count, 0, first * sizeof(uint32_t));
    }
  }
} // namespace

BOOST_AUTO_TEST_CASE(test_all_2_pow_range_up_to_double)
{
  using Table = Bin2Chars::Constants::Tables::Floating<double>;

  std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> SIMD{}, REGULAR{};

  for(int exponent = Table::MIN_BIN_EXP; exponent <= Table::MAX_BIN_EXP; exponent++)
  {
    if(exponent >= 0)
    {
      Algos::Compute::DecimalExpansion::PositiveExponent(SIMD, exponent);
      compute_pos_exponent_safe(REGULAR, exponent);
    }
    else
    {
      Algos::Compute::DecimalExpansion::NegativeExponent(SIMD, std::abs(exponent));
      compute_neg_exponent_safe(REGULAR, std::abs(exponent));
    }

    if(SIMD != REGULAR)
    {
      BOOST_CHECK(SIMD == REGULAR);
      // exponent--; // loopback
    }
  }
}
