#define BOOST_TEST_MODULE ComputeDecimalTests
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "include/Algos/Compute/ExponentDecimalExpansion.hpp"
#include "include/Constants/Constants.hpp"
#include "include/Tables/array_2n.hpp"
#include "include/Tables/array_5n.hpp"

namespace
{
  void compute_pos_exponent_safe(std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> &RESULT, const int &k)
  {
    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    RESULT.fill(0);
    RESULT[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 2 in each iteration)
    int i = 0;
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

    const int miss = k - i;
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

  void load_neg_exponent_from_table(std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> &RESULT, const int &k)
  {
    RESULT.fill(0);
    const auto *it_beg = &Bin2Chars::Tables::NegativeExponent::TABLE[0] + Bin2Chars::Tables::NegativeExponent::INDICES[k];
    const auto *it_end = &Bin2Chars::Tables::NegativeExponent::TABLE[0] + Bin2Chars::Tables::NegativeExponent::INDICES[k + 1];

    size_t i = 0;
    for(const auto *it = it_beg; it < it_end; it++)
    {
      RESULT[i++] = *it;
    }
  }

  void load_pos_exponent_from_table(std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> &RESULT, const int &k)
  {
    RESULT.fill(0);
    const auto *it_beg = &Bin2Chars::Tables::PositiveExponent::TABLE[0] + Bin2Chars::Tables::PositiveExponent::INDICES[k];
    const auto *it_end = &Bin2Chars::Tables::PositiveExponent::TABLE[0] + Bin2Chars::Tables::PositiveExponent::INDICES[k + 1];

    size_t i = 0;
    for(const auto *it = it_beg; it < it_end; it++)
    {
      RESULT[i++] = *it;
    }
  }

} // namespace

BOOST_AUTO_TEST_CASE(test_all_2_pow_range_up_to_double)
{
  using Table = Algos::Compute::DecimalExpansion::Traits<double>;

  std::array<unsigned, Algos::Compute::DecimalExpansion::MAX_ARRAY_SIZE> SIMD{}, REGULAR{}, TABLE{};

  for(int exponent = Table::MIN_BIN_EXP; exponent <= Table::MAX_BIN_EXP; exponent++)
  {
    if(exponent >= 0)
    {
      Algos::Compute::DecimalExpansion::PositiveExponent(SIMD, exponent);
      compute_pos_exponent_safe(REGULAR, exponent);
      load_pos_exponent_from_table(TABLE, exponent);
    }
    else
    {
      Algos::Compute::DecimalExpansion::NegativeExponent(SIMD, std::abs(exponent));
      compute_neg_exponent_safe(REGULAR, std::abs(exponent));
      load_neg_exponent_from_table(TABLE, std::abs(exponent));
    }

    if(SIMD != REGULAR || REGULAR != TABLE)
    {
      BOOST_CHECK(SIMD == REGULAR);
      BOOST_CHECK(TABLE == REGULAR);
      // exponent--; // loopback
    }
  }
}
