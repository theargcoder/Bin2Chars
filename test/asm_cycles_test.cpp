#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include <boost/range/numeric.hpp>

#include "include/Helpers/Assembly.hpp"

template <size_t BitWidth>
struct LaneType
{
  using type = std::conditional_t<BitWidth == 8, uint16_t,
                                  std::conditional_t<BitWidth == 16, uint16_t, std::conditional_t<BitWidth == 32, uint32_t, std::conditional_t<BitWidth == 64, uint64_t, void>>>>;
};

/**
 * @brief Extracts lanes from __m128i, __m256i, or __m512i registers into a std::array.
 * @tparam BitWidth The size of the lane in bits (8, 16, 32, 64).
 * @param reg The SIMD register to extract from.
 */
template <size_t BitWidth, typename RegType>
inline auto extract_lanes(const RegType &reg)
{
  constexpr size_t total_bytes = sizeof(RegType);
  constexpr size_t num_lanes = (total_bytes * 8) / BitWidth;
  using NumOfElemns = typename LaneType<BitWidth>::type;

  std::array<NumOfElemns, num_lanes> result;

  if constexpr(BitWidth == 8)
  {
    // Intermediary buffer to avoid debugger ASCII char issues
    alignas(RegType) uint8_t buffer[total_bytes];

    if constexpr(sizeof(RegType) == sizeof(__m64))
    {
      _mm_stream_pi(reinterpret_cast<__m64 *>(buffer), reg);
    }
    else if constexpr(sizeof(RegType) == sizeof(__m128i))
    {
      _mm_storeu_si128(reinterpret_cast<__m128i *>(buffer), reg);
    }
    else if constexpr(sizeof(RegType) == sizeof(__m256i))
    {
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(buffer), reg);
    }
    else if constexpr(sizeof(RegType) == sizeof(__m512i))
    {
      _mm512_storeu_si512(reinterpret_cast<void *>(buffer), reg);
    }

    for(size_t i = 0; i < num_lanes; ++i)
    {
      result[i] = static_cast<uint16_t>(buffer[i]);
    }
  }
  else
  {
    // Direct store for 16, 32, and 64-bit lanes
    if constexpr(sizeof(RegType) == sizeof(__m64))
    {
      _mm_stream_pi(reinterpret_cast<__m64 *>(static_cast<void *>(result.data())), reg);
    }
    else if constexpr(sizeof(RegType) == sizeof(__m128i))
    {
      _mm_storeu_si128(reinterpret_cast<__m128i *>(static_cast<void *>(result.data())), reg);
    }
    else if constexpr(sizeof(RegType) == sizeof(__m256i))
    {
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(static_cast<void *>(result.data())), reg);
    }
    else if constexpr(sizeof(RegType) == sizeof(__m512i))
    {
      _mm512_storeu_si512(reinterpret_cast<void *>(static_cast<void *>(result.data())), reg);
    }
  }

  return result;
}

using uint128_t = __uint128_t;

static constexpr uint128_t ENTRY(uint64_t digits, uint64_t offset) noexcept
{
  return (static_cast<uint128_t>(digits) << 64U) - offset;
}

template <typename T>
  requires std::is_integral_v<T> && std::is_unsigned_v<T>
struct LenTable;

template <>
struct LenTable<uint64_t>
{
  // clang-format off
    static constexpr uint128_t TABLE[] = { ENTRY(1, 0), ENTRY(1, 0), ENTRY(1, 0), ENTRY(2, 10), ENTRY(2, 10), ENTRY(2, 10), ENTRY(3, 100), ENTRY(3, 100), ENTRY(3, 100), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(4, 1000), ENTRY(5, 10000), ENTRY(5, 10000), ENTRY(5, 10000), ENTRY(6, 100000), ENTRY(6, 100000), ENTRY(6, 100000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(7, 1000000), ENTRY(8, 10000000), ENTRY(8, 10000000), ENTRY(8, 10000000), ENTRY(9, 100000000), ENTRY(9, 100000000), ENTRY(9, 100000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(10, 1000000000), ENTRY(11, 10000000000ULL), ENTRY(11, 10000000000ULL), ENTRY(11, 10000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(12, 100000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(13, 1000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(14, 10000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(15, 100000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(16, 1000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(17, 10000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(18, 100000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(19, 1000000000000000000ULL), ENTRY(20, 10000000000000000000ULL) };
  // clang-format on
};

template <>
struct LenTable<uint32_t>
{
  static constexpr uint64_t TABLE[] = { 4294967296ULL,  8589934582ULL,  8589934582ULL,  8589934582ULL,  12884901788ULL, 12884901788ULL, 12884901788ULL, 17179868184ULL,
                                        17179868184ULL, 17179868184ULL, 21474826480ULL, 21474826480ULL, 21474826480ULL, 21474826480ULL, 25769703776ULL, 25769703776ULL,
                                        25769703776ULL, 30063771072ULL, 30063771072ULL, 30063771072ULL, 34349738368ULL, 34349738368ULL, 34349738368ULL, 34349738368ULL,
                                        38554705664ULL, 38554705664ULL, 38554705664ULL, 41949672960ULL, 41949672960ULL, 41949672960ULL, 42949672960ULL, 42949672960ULL };
};

template <>
struct LenTable<uint16_t>
{
  static constexpr uint32_t TABLE[] = { 65536, 65536, 65536, 131062, 131072, 131072, 196508, 196608, 196608, 261144, 262144, 262144, 262144, 317680, 327680, 327680 };
};

template <typename T>
  requires std::is_integral_v<T> && std::is_unsigned_v<T>
inline __attribute__((always_inline)) static unsigned calculate_len(const T &input)
{
  using LEN_TABLE = LenTable<T>;
  if constexpr(std::is_same_v<T, uint64_t>)
  {
    const unsigned comp = 63U - static_cast<unsigned>(__builtin_clzll(input | 1ULL));
    return static_cast<unsigned>((static_cast<uint128_t>(input) + LEN_TABLE::TABLE[comp]) >> 64U);
  }
  else if constexpr(std::is_same_v<T, uint32_t>)
  {
    const unsigned comp = 31U - static_cast<unsigned>(__builtin_clz(input | 1U));
    return static_cast<unsigned>((input + LEN_TABLE::TABLE[comp]) >> 32U);
  }
  else if constexpr(std::is_same_v<T, uint16_t>)
  {
    const unsigned comp = 31U - static_cast<unsigned>(__builtin_clz(input | 1U));
    return static_cast<unsigned>((input + LEN_TABLE::TABLE[comp]) >> 16U);
  }
}

template <typename T>
[[gnu::noinline, gnu::used]] unsigned WriteCharsToPtrFowardReturnLength(char *__restrict__ buff, const T &input) noexcept
{
  asm volatile("# LLVM-MCA-BEGIN SIMD_WriteChars");

  constexpr uint32_t MAGIC_u32[] = { 0x55E63B89U, 0x431BDE83U, 0xD1B71759U, 0x51EB851FU };
  constexpr uint32_t SHIFTS_u32[] = { 57, 50, 45, 37 };
  constexpr uint8_t IND_u16[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

  const __m256i VAL = _mm256_set1_epi32(static_cast<int32_t>(input));

  const __m256i M_MAGIC_u64 = _mm256_cvtepi32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i_u *>(static_cast<const void *>(&MAGIC_u32[0]))));
  const __m256i M_SHIFTS_u64 = _mm256_cvtepi32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i_u *>(static_cast<const void *>(&SHIFTS_u32[0]))));

  const __m128i INDICES = _mm_loadu_si128(reinterpret_cast<const __m128i_u *>(static_cast<const void *>(&IND_u16[0])));

  const __m256i prod = _mm256_mul_epu32(VAL, M_MAGIC_u64);

  const unsigned len = calculate_len(input);
  const unsigned lead_z = 10U - len;

  const __m256i shifted = _mm256_srlv_epi64(prod, M_SHIFTS_u64);
  const __m128i shifted_64 = _mm256_castsi256_si128(_mm256_permute4x64_epi64(_mm256_shuffle_epi32(shifted, _MM_SHUFFLE(1, 3, 2, 0)), 0b11'01'10'00));

  const __m128i shifted_64_x_2 = _mm_slli_epi32(shifted_64, 1);
  const __m128i shifted_64_x_3 = _mm_add_epi32(shifted_64, shifted_64_x_2);
  const __m128i shifted_64_x_24 = _mm_slli_epi32(shifted_64_x_3, 3);
  const __m128i shifted_64_x_25 = _mm_add_epi32(shifted_64_x_24, shifted_64);
  const __m128i shifted_64_x_100 = _mm_slli_epi32(shifted_64_x_25, 2);

  const __m128i top_lanes = _mm_slli_si128(shifted_64_x_100, 4);
  const __m128i bot_lanes = _mm_shuffle_epi32(shifted_64_x_100, _MM_SHUFFLE(0, 1, 2, 3));

  const __m128i F_6554 = _mm_set1_epi16(6554);

  const __m128i res_u64_top = _mm_sub_epi64(shifted_64, top_lanes);
  const __m128i res_u64_bot = _mm_sub_epi64(_mm256_castsi256_si128(VAL), bot_lanes);

  const __m128i shifted_top_16 = _mm_slli_epi64(res_u64_top, 16);
  const __m128i shifted_bot_16 = _mm_slli_epi64(res_u64_bot, 16);

  const __m128i res_packed_top = _mm_or_si128(shifted_top_16, res_u64_top);
  const __m128i res_packed_bot = _mm_or_si128(shifted_bot_16, res_u64_bot);

  const __m128i res_prod_top = _mm_mulhi_epu16(res_packed_top, F_6554);
  const __m128i res_prod_bot = _mm_mulhi_epu16(res_packed_bot, F_6554);

  const __m128i res_shifted_top_x8 = _mm_slli_epi16(res_prod_top, 3);
  const __m128i res_shifted_bot_x8 = _mm_slli_epi16(res_prod_bot, 3);
  const __m128i res_shifted_top_x2 = _mm_slli_epi16(res_prod_top, 1);
  const __m128i res_shifted_bot_x2 = _mm_slli_epi16(res_prod_bot, 1);

  const __m128i res_shifted_top_x10 = _mm_add_epi16(res_shifted_top_x8, res_shifted_top_x2);
  const __m128i res_shifted_bot_x10 = _mm_add_epi16(res_shifted_bot_x8, res_shifted_bot_x2);

  const __m128i ZERO_NUMS = _mm_setzero_si128();
  const __m128i ZERO_CHAR = _mm_set1_epi8('0');
  const __m128i LEAD_Z_LANES = _mm_set1_epi8(static_cast<int8_t>(lead_z));

  const __m128i res_shf_blen_top = _mm_blend_epi16(res_shifted_top_x10, ZERO_NUMS, 0b0101'0101);
  const __m128i res_shf_blen_bot = _mm_blend_epi16(res_shifted_bot_x10, ZERO_NUMS, 0b0101'0101);
  const __m128i res_to_sub_top = _mm_blend_epi16(res_prod_top, res_packed_top, 0b1010'1010);
  const __m128i res_to_sub_bot = _mm_blend_epi16(res_prod_bot, res_packed_bot, 0b1010'1010);
  const __m128i res_com_top = _mm_sub_epi16(res_to_sub_top, res_shf_blen_top);
  const __m128i res_com_bot = _mm_sub_epi16(res_to_sub_bot, res_shf_blen_bot);

  const __m128i trunc_u8 = _mm_packus_epi16(res_com_top, res_com_bot);

  const __m128i ascii_vec = _mm_add_epi8(trunc_u8, ZERO_CHAR);
  const __m128i final_indices = _mm_add_epi8(INDICES, LEAD_Z_LANES);
  const __m128i output_chars = _mm_shuffle_epi8(ascii_vec, final_indices);

  _mm_storeu_si64(reinterpret_cast<void *>(buff), output_chars);

  const __m128i top_top = _mm_srli_si128(output_chars, 8);

  _mm_storeu_si16(reinterpret_cast<void *>(buff + 8), top_top);

  asm volatile("# LLVM-MCA-END SIMD_WriteChars");

  return len;
}

constexpr char digits[201] = "0001020304050607080910111213141516171819"
                             "2021222324252627282930313233343536373839"
                             "4041424344454647484950515253545556575859"
                             "6061626364656667686970717273747576777879"
                             "8081828384858687888990919293949596979899";
template <typename T>
[[gnu::noinline, gnu::used]] unsigned StdLibOptimized(char *__restrict__ buff, const T &input) noexcept
{
  const unsigned len = calculate_len(input);
  unsigned pos = len - 1;
  auto val = input;
  while(val >= 100)
  {
    auto const num = (val % 100) * 2;
    val /= 100;
    buff[pos] = digits[num + 1];
    buff[pos - 1] = digits[num];
    pos -= 2;
  }

  if(val >= 10)
  {
    auto const num = val * 2;
    buff[1] = digits[num + 1];
    buff[0] = digits[num];
  }
  else
  {
    buff[0] = static_cast<char>('0' + val);
  }

  return len;
}

int main(int /*unused*/, char ** /*unused*/)
{
  // 1. Pin to a specific core to avoid cross-core TSC sync issues
  Bin2Chars::Helpers::Assembly::pin_thread_to_cpu(3);

  using test_t = uint32_t;

  static constexpr size_t TRIALS = 10'000'000;
  std::vector<test_t> random_inputs(TRIALS);
  std::vector<uint64_t> simdy_times(TRIALS);
  std::vector<uint64_t> std_times(TRIALS);

  // 2. Pre-generate randoms to completely destroy std::to_string branch prediction
  std::mt19937 rng(123'456'678U);
  std::uniform_int_distribution<test_t> dist(0, static_cast<test_t>(std::numeric_limits<test_t>::max())); // Mix of digits
  for(size_t i = 0; i < TRIALS; ++i)
  {
    random_inputs[i] = dist(rng);
  }

  char buff[65];

  Bin2Chars::Helpers::Assembly::prefetch_elements<64>(&buff[0]);

  // 3. The Measurement Loop
  for(size_t i = 0; i < TRIALS; ++i)
  {
    _mm_clflush(buff);
    _mm_mfence();
    Bin2Chars::Helpers::Assembly::prefetch_elements<64>(&buff[0]);
    const auto current_num = static_cast<test_t>(random_inputs[i]);

    const uint64_t st_simdy = Bin2Chars::Helpers::Assembly::timer_start();

    const auto len = WriteCharsToPtrFowardReturnLength<test_t>(&buff[0], current_num);

    const uint64_t en_simdy = Bin2Chars::Helpers::Assembly::timer_end();

    // Force compiler to materialize the result
    asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "r"(len) : "memory");

    simdy_times[i] = en_simdy - st_simdy;

    _mm_clflush(buff);
    _mm_mfence();
    Bin2Chars::Helpers::Assembly::prefetch_elements<64>(&buff[0]);

    const uint64_t st_std = Bin2Chars::Helpers::Assembly::timer_start();

    const auto pp = StdLibOptimized<test_t>(&buff[0], current_num);

    const uint64_t en_std = Bin2Chars::Helpers::Assembly::timer_end();

    // Force compiler to materialize the result
    asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "r"(pp) : "memory");
    std_times[i] = en_std - st_std;
  }

  // 4. Statistical Analysis
  std::ranges::sort(simdy_times);
  std::ranges::sort(std_times);

  const auto simdy_accum = boost::accumulate(simdy_times, 0ULL);
  const auto std_accum = boost::accumulate(std_times, 0ULL);

  // The Minimum is the "perfect" hardware run.
  // The Median is the true realistic "Cold" run, ignoring OS interrupts.
  printf("\n=== PERFECT STATS (Cold Data, Unpredictable Branches, %ld runs) ===\n", TRIALS);
  printf("SIMDY     | Min: %4lu | Median: %4lu | Mean: %.3f | 95th Percentile: %4lu\n", simdy_times[0], simdy_times[TRIALS / 2],
         static_cast<double>(simdy_accum) / static_cast<double>(simdy_times.size()), simdy_times[TRIALS * 95 / 100]);
  printf("TO_STRING | Min: %4lu | Median: %4lu | Mean: %.3f | 95th Percentile: %4lu\n", std_times[0], std_times[TRIALS / 2],
         static_cast<double>(std_accum) / static_cast<double>(std_times.size()), std_times[TRIALS * 95 / 100]);

  return 0;
}
