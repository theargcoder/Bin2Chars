#define BOOST_TEST_MODULE ComputeDecimalTests
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "include/Tables/array_2n.hpp"
#include "include/Tables/array_5n.hpp"

namespace
{
  constexpr auto TABLE_SIZE = 96;
  void compute_pos_exponent_safe(std::array<unsigned, TABLE_SIZE> &RESULT, const int &k)
  {
    constexpr uint32_t MOD_10E9 = 1'000'000'000U;

    RESULT.fill(0);
    RESULT[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 32 in each iteration)
    int i;
    for(i = 0; i + 5 < k; i += 5)
    {
      uint32_t carry = 0;
      for(unsigned int &w : RESULT)
      {
        uint64_t pp = (static_cast<uint64_t>(w) << 5U) + carry;

        // Let the compiler synthesize the optimal 128-bit magic multiplication
        carry = static_cast<uint32_t>(pp / MOD_10E9);
        w = static_cast<uint32_t>(pp - carry * MOD_10E9);
      }
    }

    const int miss = k - i;
    uint32_t carry = 0;
    for(unsigned int &w : RESULT)
    {
      uint64_t pp = (static_cast<uint64_t>(w) << miss) + carry;
      carry = static_cast<uint32_t>(pp / MOD_10E9);
      w = static_cast<uint32_t>(pp - carry * MOD_10E9);
    }
  }

  void compute_neg_exponent_safe(std::array<unsigned, TABLE_SIZE> &RESULT, const int &k)
  {
    RESULT.fill(0);
    RESULT[0] = 1; // Initialize 5^0 = 1

    // 5^1074 has ~751 decimal digits. This requires 84 words in base-10^9.
    constexpr uint32_t R10E9 = 1'000'000'000U;

    const uint32_t pairs = static_cast<uint32_t>(k) / 2; // 537 passes of x25
    for(uint32_t i = 0; i < pairs; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : RESULT)
      {
        uint64_t pp = static_cast<uint64_t>(w) * 25U + carry;
        carry = static_cast<uint32_t>(pp / R10E9);
        w = static_cast<uint32_t>(pp - carry * R10E9);
      }
    }

    // Handle odd exponent leftover (5^1)
    if(k % 2 != 0)
    {
      uint32_t carry = 0;
      for(unsigned int &w : RESULT)
      {
        uint64_t pp = static_cast<uint64_t>(w) * 5U + carry;
        carry = static_cast<uint32_t>(pp / R10E9);
        w = static_cast<uint32_t>(pp - carry * R10E9);
      }
    }
  }

  void load_neg_exponent_from_table(std::array<unsigned, TABLE_SIZE> &RESULT, const int &k)
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

  void load_pos_exponent_from_table(std::array<unsigned, TABLE_SIZE> &RESULT, const int &k)
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
  std::array<unsigned, TABLE_SIZE> COMPUTATION{}, TABLE{};

  for(int exponent = -1074; exponent <= 1024; exponent++)
  {
    if(exponent >= 0)
    {
      compute_pos_exponent_safe(COMPUTATION, exponent);
      load_pos_exponent_from_table(TABLE, exponent);
    }
    else
    {
      compute_neg_exponent_safe(COMPUTATION, std::abs(exponent));
      load_neg_exponent_from_table(TABLE, std::abs(exponent));
    }

    if(COMPUTATION != TABLE)
    {
#ifdef BIN2CHARS_CIBUILD
      BOOST_REQUIRE(TABLE == REGULAR);
#else
      BOOST_CHECK(TABLE == COMPUTATION);
#endif
      // exponent--; // loopback
    }
  }
}
