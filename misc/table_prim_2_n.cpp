#include <array>
#include <cassert>
#include <cmath>
#include <cstdint> // Added for explicit uint32_t and uint64_t types
#include <cstring>
#include <emmintrin.h>
#include <iostream>

#if defined(_MSC_VER) || defined(__x86_64__) || defined(__i386__)
#include <immintrin.h> // x86 SIMD
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h> // ARM SIMD
#endif

int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t POW_5_E[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm256i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm256i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm256i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m256i ZERO = _mm256_setzero_si256();

    // Total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 28> SIMD_ARRAY;
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), ZERO);

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 4U])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 12U])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 20U])));
    __m256i rrprime_7 = ZERO;
    const __m256i R1E8 = _mm256_set1_epi64x(POW_5_CORRECTION[8]);
    const __m256i R1REMMIS = _mm256_set1_epi64x(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned E_o = POW_5_E[P_DIV_32];

    for(unsigned E = E_o; E < P_DIV_8; E++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1E8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1E8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1E8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1E8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1E8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1E8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1E8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
    }
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1REMMIS);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1REMMIS);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1REMMIS);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1REMMIS);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1REMMIS);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1REMMIS);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1REMMIS);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
    }

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), rrprime_2);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), rrprime_3);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), rrprime_4);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), rrprime_5);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), rrprime_6);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), rrprime_7);

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_7);
    }

    SLOW_PRE_ARR[0] = 1; // Initialize R = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t p = (uint64_t)w * 10ULL + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    std::array<uint32_t, NUM_WORDS> X10byx10_ARR = SLOW_PRE_ARR;

    size_t first = 0;

    while(first < SLOW_PRE_ARR.size() && SLOW_PRE_ARR[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != SLOW_PRE_ARR.size())
    {
      const size_t count = SLOW_PRE_ARR.size() - first;

      std::memmove(SLOW_PRE_ARR.data(), SLOW_PRE_ARR.data() + first, count * sizeof(uint32_t));

      std::memset(SLOW_PRE_ARR.data() + count, 0, first * sizeof(uint32_t));
    }

    if(SIMD_PRE_WRITTEN != SLOW_PRE_ARR)
    {
      std::cout << "k = " << k << "; P = " << P << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    // unsigned lead_z = 24U - (K_DIV_32 - ((k * 1233U) >> 17U)) - (k < 32); //+ (K_DIV_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32U;
    unsigned simdy_idx = (K_DIV_32 - ((k * 1233U) >> 17U));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < total_digits)
    {
      // Step A & B: Multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // Carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : X10byx10_ARR)
      {
        // Max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. Fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000U + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      {
        __m256i rrrprime_1 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]));
        __m256i rrrprime_2 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]));
        __m256i rrrprime_3 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]));
        __m256i rrrprime_4 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]));
        __m256i rrrprime_5 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]));
        __m256i rrrprime_6 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]));
        __m256i rrrprime_7 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]));
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrrprime_1, R1E8);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrrprime_2, R1E8);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrrprime_3, R1E8);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrrprime_4, R1E8);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrrprime_5, R1E8);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrrprime_6, R1E8);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrrprime_7, R1E8);
        const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
        const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
        const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
        const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
        const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
        const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
        const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
        const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        rrrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
        rrrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
        rrrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
        rrrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
        rrrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
        rrrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
        rrrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
        const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_1, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_2, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_3, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_4, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_5, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_6, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrrprime_7, 0b10'01'00'11), 32U);
        const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
        const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
        const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
        const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
        const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
        const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
        const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
        rrrprime_1 = _mm256_add_epi32(rrrprime_1, rrprime_slide_1);
        rrrprime_2 = _mm256_add_epi32(rrrprime_2, rrprime_slide_2);
        rrrprime_3 = _mm256_add_epi32(rrrprime_3, rrprime_slide_3);
        rrrprime_4 = _mm256_add_epi32(rrrprime_4, rrprime_slide_4);
        rrrprime_5 = _mm256_add_epi32(rrrprime_5, rrprime_slide_5);
        rrrprime_6 = _mm256_add_epi32(rrrprime_6, rrprime_slide_6);
        rrrprime_7 = _mm256_add_epi32(rrrprime_7, rrprime_slide_7);
        rrrprime_1 = _mm256_blend_epi32(rrrprime_1, ZERO, 0b1010'1010);
        rrrprime_2 = _mm256_blend_epi32(rrrprime_2, ZERO, 0b1010'1010);
        rrrprime_3 = _mm256_blend_epi32(rrrprime_3, ZERO, 0b1010'1010);
        rrrprime_4 = _mm256_blend_epi32(rrrprime_4, ZERO, 0b1010'1010);
        rrrprime_5 = _mm256_blend_epi32(rrrprime_5, ZERO, 0b1010'1010);
        rrrprime_6 = _mm256_blend_epi32(rrrprime_6, ZERO, 0b1010'1010);
        rrrprime_7 = _mm256_blend_epi32(rrrprime_7, ZERO, 0b1010'1010);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), rrrprime_1);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), rrrprime_2);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), rrrprime_3);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), rrrprime_4);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), rrrprime_5);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), rrrprime_6);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), rrrprime_7);
      }

      // Step C: Extract next8 chunk (R >> k)
      chunk = X10byx10_ARR[word_idx] >> bit_shift;
      // Cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)X10byx10_ARR[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      X10byx10_ARR[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&X10byx10_ARR[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      chunk = SIMD_ARRAY[simdy_idx] >> bit_shift;
      chunk |= (SIMD_ARRAY[simdy_idx + 1] << (32 - bit_shift));
      simdy_8 = (uint32_t)chunk;
      SIMD_ARRAY[simdy_idx] &= mod_mask;
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[simdy_idx + 1]), ZERO);

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; P = " << P << '\n';
        goto backwards;
      }

      assert(simdy_8 == next_8);

      // Step E: Print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }
    //
  }

  return 0;
}

/*
int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t POW_5_E[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm256i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm256i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm256i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m256i ZERO = _mm256_setzero_si256();

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 4U])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 12U])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 20U])));
    __m256i rrprime_7 = ZERO;
    __m256i rrprime_8 = ZERO;
    const __m256i R1E8 = _mm256_set1_epi64x(POW_5_CORRECTION[8]);
    const __m256i R1REMMIS = _mm256_set1_epi64x(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned E_o = POW_5_E[P_DIV_32];

    for(unsigned E = E_o; E < P_DIV_8; E++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1E8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1E8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1E8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1E8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1E8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1E8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1E8);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R1E8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1REMMIS);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1REMMIS);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1REMMIS);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1REMMIS);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1REMMIS);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1REMMIS);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1REMMIS);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R1REMMIS);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }

    // Total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> written = { 0 };
    std::array<uint32_t, NUM_WORDS> r = { 0 };

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_8 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_8, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[24]), u32_pack_7);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[28]), u32_pack_8);
    }

    r[0] = 1; // Initialize R = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : r)
      {
        uint64_t p = (uint64_t)w * 10ULL + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    size_t first = 0;

    while(first < r.size() && r[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != r.size())
    {
      const size_t count = r.size() - first;

      std::memmove(r.data(), r.data() + first, count * sizeof(uint32_t));

      std::memset(r.data() + count, 0, first * sizeof(uint32_t));
    }

    if(written != r)
    {
      std::cout << "k = " << k << "; P = " << P << '\n';
      goto backwards;
    }

    assert(written == r);

    unsigned lead_z = 24U - (K_DIV_32 - ((k * 1233U) >> 17U)) - (k < 32); //+ (K_DIV_32 & 0b0111);

    while(lead_z != 0)
    {
      if(lead_z >= 16)
      {
        rrprime_8 = rrprime_4;
        rrprime_7 = rrprime_3;
        rrprime_6 = rrprime_2;
        rrprime_5 = rrprime_1;
        rrprime_4 = ZERO;
        rrprime_3 = ZERO;
        rrprime_2 = ZERO;
        rrprime_1 = ZERO;
        lead_z -= 16U;
      }
      else if(lead_z >= 8)
      {
        rrprime_8 = rrprime_6;
        rrprime_7 = rrprime_5;
        rrprime_6 = rrprime_4;
        rrprime_5 = rrprime_3;
        rrprime_4 = rrprime_2;
        rrprime_3 = rrprime_1;
        rrprime_2 = ZERO;
        rrprime_1 = ZERO;

        lead_z -= 8U;
      }
      else if(lead_z >= 4)
      {
        rrprime_8 = rrprime_7;
        rrprime_7 = rrprime_6;
        rrprime_6 = rrprime_5;
        rrprime_5 = rrprime_4;
        rrprime_4 = rrprime_3;
        rrprime_3 = rrprime_2;
        rrprime_2 = rrprime_1;
        rrprime_1 = ZERO;

        lead_z -= 4U;
      }
      else if(lead_z >= 2)
      {
        const __m256i perm_8 = _mm256_permute4x64_epi64(rrprime_8, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_7 = _mm256_permute4x64_epi64(rrprime_7, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_6 = _mm256_permute4x64_epi64(rrprime_6, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_5 = _mm256_permute4x64_epi64(rrprime_5, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_4 = _mm256_permute4x64_epi64(rrprime_4, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_3 = _mm256_permute4x64_epi64(rrprime_3, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_2 = _mm256_permute4x64_epi64(rrprime_2, _MM_SHUFFLE(1, 0, 3, 2));
        const __m256i perm_1 = _mm256_permute4x64_epi64(rrprime_1, _MM_SHUFFLE(1, 0, 3, 2));

        rrprime_1 = _mm256_blend_epi32(perm_1, ZERO, 0b00001111);
        rrprime_2 = _mm256_blend_epi32(perm_2, perm_1, 0b00001111);
        rrprime_3 = _mm256_blend_epi32(perm_3, perm_2, 0b00001111);
        rrprime_4 = _mm256_blend_epi32(perm_4, perm_3, 0b00001111);
        rrprime_5 = _mm256_blend_epi32(perm_5, perm_4, 0b00001111);
        rrprime_6 = _mm256_blend_epi32(perm_6, perm_5, 0b00001111);
        rrprime_7 = _mm256_blend_epi32(perm_7, perm_6, 0b00001111);
        rrprime_8 = _mm256_blend_epi32(perm_8, perm_7, 0b00001111);

        lead_z -= 2U;
      }
      else
      {
        const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11);
        rrprime_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
        rrprime_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        rrprime_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        rrprime_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        rrprime_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        rrprime_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        rrprime_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        rrprime_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
        lead_z -= 1U;
      }
    }

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32U;
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1U << 6U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < total_digits)
    {
      // Step A & B: Multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // Carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : r)
      {
        // Max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. Fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000U + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      {
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1E8);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1E8);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1E8);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1E8);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1E8);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1E8);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1E8);
        const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R1E8);
        const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
        const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, ZERO, 0b1010'1010);
        const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
        const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
        const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
        const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
        const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
        const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
        const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
        const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
        const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
        rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
        rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
        rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
        rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
        rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
        rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
        rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
        rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
        const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
        const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32U);
        const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
        const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
        const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
        const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
        const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
        const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
        const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
        const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
        rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
        rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
        rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
        rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
        rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
        rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
        rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
        rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
      }

      const __m128i rrprime_7_128 = _mm256_castsi256_si128(rrprime_7);

      const __m128i next_o = _mm_shuffle_epi32(rrprime_7_128, 0b01'00'11'10);

      const __m128i lo_o = _mm_srli_epi64(rrprime_7_128, bit_shift);
      const __m128i hi_o = _mm_slli_epi64(next_o, 32U - bit_shift);
      const __m128i chunks_o = _mm_or_si128(lo_o, hi_o);

      // Step C: Extract next8 chunk (R >> k)
      chunk = 0;
      if(word_idx < NUM_WORDS)
      {
        chunk = r[word_idx] >> bit_shift;
      }
      if(word_idx + 1 < NUM_WORDS && bit_shift != 0)
      {
        // Cast to uint64_t before shifting to prevent any 32-bit boundary overflow
        chunk |= ((uint64_t)r[word_idx + 1] << (32 - bit_shift));
      }
      next_8 = (uint32_t)chunk;

      // Step D: Apply Modulo 2^k instantly
      if(word_idx < NUM_WORDS)
      {
        r[word_idx] &= mod_mask;
        std::memset(static_cast<void *>(&r[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);
      }

      const unsigned r_wrd_idx = _mm256_extract_epi32(rrprime_6, 6);
      const unsigned r_wrd_idx_plus_1 = _mm256_extract_epi32(rrprime_7, 0);

      const unsigned r_pt_1 = ((r_wrd_idx) >> bit_shift);
      const unsigned r_pt_2 = (r_wrd_idx_plus_1 << (32 - bit_shift));

      simdy_8 = r_pt_1 | r_pt_2;

      const unsigned r_wrd_idx_masked = r_wrd_idx & mod_mask;

      const __m256i rrprime_7_modd = _mm256_set1_epi64x(r_wrd_idx_masked);
      rrprime_6 = _mm256_blend_epi32(rrprime_6, rrprime_7_modd, 0b11'00'00'00);
      rrprime_7 = ZERO;

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; P = " << P << '\n';
        goto backwards;
      }

      assert(simdy_8 == next_8);

      // Step E: Print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }
    //
  }

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t POW_5_E[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm256i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm256i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm256i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm256i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm256i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m256i ZERO = _mm256_setzero_si256();

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 4U])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 12U])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 20U])));
    __m256i rrprime_7 = ZERO;
    __m256i rrprime_8 = ZERO;
    const __m256i R1E8 = _mm256_set1_epi64x(POW_5_CORRECTION[8]);
    const __m256i R1REMMIS = _mm256_set1_epi64x(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned E_o = POW_5_E[P_DIV_32];

    for(unsigned E = E_o; E < P_DIV_8; E++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1E8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1E8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1E8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1E8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1E8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1E8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1E8);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R1E8);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1REMMIS);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1REMMIS);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1REMMIS);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1REMMIS);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1REMMIS);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1REMMIS);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1REMMIS);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R1REMMIS);
      const __m256i u32_lo_prod_1 = _mm256_blend_epi32(u64_prod_1, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_2 = _mm256_blend_epi32(u64_prod_2, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_3 = _mm256_blend_epi32(u64_prod_3, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_4 = _mm256_blend_epi32(u64_prod_4, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_5 = _mm256_blend_epi32(u64_prod_5, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_6 = _mm256_blend_epi32(u64_prod_6, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_7 = _mm256_blend_epi32(u64_prod_7, ZERO, 0b1010'1010);
      const __m256i u32_lo_prod_8 = _mm256_blend_epi32(u64_prod_8, ZERO, 0b1010'1010);
      const __m256i u32_hi_prod_1 = _mm256_srli_epi64(u64_prod_1, 32);
      const __m256i u32_hi_prod_2 = _mm256_srli_epi64(u64_prod_2, 32);
      const __m256i u32_hi_prod_3 = _mm256_srli_epi64(u64_prod_3, 32);
      const __m256i u32_hi_prod_4 = _mm256_srli_epi64(u64_prod_4, 32);
      const __m256i u32_hi_prod_5 = _mm256_srli_epi64(u64_prod_5, 32);
      const __m256i u32_hi_prod_6 = _mm256_srli_epi64(u64_prod_6, 32);
      const __m256i u32_hi_prod_7 = _mm256_srli_epi64(u64_prod_7, 32);
      const __m256i u32_hi_prod_8 = _mm256_srli_epi64(u64_prod_8, 32);
      const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u32_hi_prod_1, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u32_hi_prod_2, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u32_hi_prod_3, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u32_hi_prod_4, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u32_hi_prod_5, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u32_hi_prod_6, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u32_hi_prod_7, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u32_hi_prod_8, 0b10'01'00'11);
      const __m256i u32_hi_prod_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
      const __m256i u32_hi_prod_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm256_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm256_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm256_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm256_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
      rrprime_6 = _mm256_add_epi64(u32_lo_prod_6, u32_hi_prod_slide_6);
      rrprime_7 = _mm256_add_epi64(u32_lo_prod_7, u32_hi_prod_slide_7);
      rrprime_8 = _mm256_add_epi64(u32_lo_prod_8, u32_hi_prod_slide_8);
      const __m256i rrprime_perm_1 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_1, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_2 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_2, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_3 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_3, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_4 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_4, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_5 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_5, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_6 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_6, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_7 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_7, 0b10'01'00'11), 32U);
      const __m256i rrprime_perm_8 = _mm256_srli_epi64(_mm256_permute4x64_epi64(rrprime_8, 0b10'01'00'11), 32U);
      const __m256i rrprime_slide_1 = _mm256_blend_epi32(rrprime_perm_1, ZERO, 0b00'00'00'11);
      const __m256i rrprime_slide_2 = _mm256_blend_epi32(rrprime_perm_2, rrprime_perm_1, 0b00'00'00'11);
      const __m256i rrprime_slide_3 = _mm256_blend_epi32(rrprime_perm_3, rrprime_perm_2, 0b00'00'00'11);
      const __m256i rrprime_slide_4 = _mm256_blend_epi32(rrprime_perm_4, rrprime_perm_3, 0b00'00'00'11);
      const __m256i rrprime_slide_5 = _mm256_blend_epi32(rrprime_perm_5, rrprime_perm_4, 0b00'00'00'11);
      const __m256i rrprime_slide_6 = _mm256_blend_epi32(rrprime_perm_6, rrprime_perm_5, 0b00'00'00'11);
      const __m256i rrprime_slide_7 = _mm256_blend_epi32(rrprime_perm_7, rrprime_perm_6, 0b00'00'00'11);
      const __m256i rrprime_slide_8 = _mm256_blend_epi32(rrprime_perm_8, rrprime_perm_7, 0b00'00'00'11);
      rrprime_1 = _mm256_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm256_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm256_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm256_add_epi32(rrprime_4, rrprime_slide_4);
      rrprime_5 = _mm256_add_epi32(rrprime_5, rrprime_slide_5);
      rrprime_6 = _mm256_add_epi32(rrprime_6, rrprime_slide_6);
      rrprime_7 = _mm256_add_epi32(rrprime_7, rrprime_slide_7);
      rrprime_8 = _mm256_add_epi32(rrprime_8, rrprime_slide_8);
    }

    // Total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> written = { 0 };
    std::array<uint32_t, NUM_WORDS> r = { 0 };

    const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_8 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_8, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[0]), u32_pack_1);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[4]), u32_pack_2);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[8]), u32_pack_3);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[12]), u32_pack_4);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[16]), u32_pack_5);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[20]), u32_pack_6);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[24]), u32_pack_7);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&written[28]), u32_pack_8);

    r[0] = 1; // Initialize R = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : r)
      {
        uint64_t p = (uint64_t)w * 10ULL + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    size_t first = 0;

    while(first < r.size() && r[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != r.size())
    {
      const size_t count = r.size() - first;

      std::memmove(r.data(), r.data() + first, count * sizeof(uint32_t));

      std::memset(r.data() + count, 0, first * sizeof(uint32_t));
    }

    if(written != r)
    {
      std::cout << "k = " << k << "; P = " << P << '\n';
      goto backwards;
    }

    assert(written == r);

    // std::cout << "\n";
  }

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t POW_5_E[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm512i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm512i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm512i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 0; k < 1075; k++)
  {
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m512i ZERO = _mm512_setzero_si512();

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&POW_5_CACHE[POW_5_IDX[P_DIV_32]]));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U]));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U]));
    __m512i rrprime_4 = ZERO;
    __m512i r1e8 = _mm512_set1_epi64(POW_5_CORRECTION[8]);
    __m512i r1emis = _mm512_set1_epi64(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned E_o = POW_5_E[P_DIV_32];

    for(unsigned E = E_o; E < P_DIV_8; E++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }
    {

      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1emis);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1emis);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1emis);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1emis);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi32(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi32(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi32(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi32(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }

    // Total digits in 5^k
    const uint32_t total_digits = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    unsigned digits_computed = 0;

    std::cout << "k = " << k << std::endl;
    uint32_t r[NUM_WORDS] = { 0 };
    r[0] = 1; // Initialize R = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : r)
      {
        uint64_t p = (uint64_t)w * 10ULL + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    const unsigned bit_shift = k & ((1U << 6U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    const __m512i mod_vec = _mm512_set1_epi64(mod_mask);

    const unsigned rrprime_1_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_1, ZERO);
    const unsigned rrprime_2_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_2, ZERO);
    const unsigned rrprime_3_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_3, ZERO);
    const unsigned rrprime_4_zero_mask = _mm512_cmpneq_epi64_mask(rrprime_4, ZERO);
    unsigned comb = rrprime_4_zero_mask << 24U | rrprime_3_zero_mask << 16U | rrprime_2_zero_mask << 8U | rrprime_1_zero_mask;
    unsigned lead_z = __builtin_clz(comb) - 8U;
    while(lead_z != 0)
    {
      if(lead_z >= 16)
      {
        rrprime_4 = rrprime_2;
        rrprime_3 = rrprime_1;
        rrprime_2 = ZERO;
        rrprime_1 = ZERO;
        comb <<= 16U;
      }
      else if(lead_z >= 8)
      {
        rrprime_4 = rrprime_3;
        rrprime_3 = rrprime_2;
        rrprime_2 = rrprime_1;
        rrprime_1 = ZERO;
        comb <<= 8U;
      }
      else if(lead_z >= 4)
      {
        rrprime_4 = _mm512_alignr_epi64(rrprime_4, rrprime_3, 4);
        rrprime_3 = _mm512_alignr_epi64(rrprime_3, rrprime_2, 4);
        rrprime_2 = _mm512_alignr_epi64(rrprime_2, rrprime_1, 4);
        rrprime_1 = _mm512_alignr_epi64(rrprime_1, ZERO, 4);
        comb <<= 4U;
      }
      else if(lead_z >= 2)
      {
        rrprime_4 = _mm512_alignr_epi64(rrprime_4, rrprime_3, 6);
        rrprime_3 = _mm512_alignr_epi64(rrprime_3, rrprime_2, 6);
        rrprime_2 = _mm512_alignr_epi64(rrprime_2, rrprime_1, 6);
        rrprime_1 = _mm512_alignr_epi64(rrprime_1, ZERO, 6);
        comb <<= 2U;
      }
      else
      {
        rrprime_4 = _mm512_alignr_epi64(rrprime_4, rrprime_3, 7);
        rrprime_3 = _mm512_alignr_epi64(rrprime_3, rrprime_2, 7);
        rrprime_2 = _mm512_alignr_epi64(rrprime_2, rrprime_1, 7);
        rrprime_1 = _mm512_alignr_epi64(rrprime_1, ZERO, 7);
        comb <<= 1U;
      }
      lead_z = __builtin_clz(comb) - 8U;
    }

    uint32_t word_idx = k / 32;
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    while(digits_computed < total_digits)
    {

      // Step A & B: Multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // Carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : r)
      {
        // Max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. Fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000U + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);

      const __m512i next_o = _mm512_alignr_epi64(rrprime_3, ZERO, 7);

      const __m512i last_lsb = _mm512_alignr_epi64(ZERO, rrprime_3, 7);

      const __m512i lo_o = _mm512_srli_epi64(rrprime_4, bit_shift);
      const __m512i hi_o = _mm512_slli_epi64(next_o, 32U - bit_shift);

      const __m512i lo_n = _mm512_srli_epi64(last_lsb, bit_shift);
      const __m512i hi_n = _mm512_slli_epi64(rrprime_4, 32U - bit_shift);

      const __m512i chunks_o = _mm512_or_si512(lo_o, hi_o);
      const __m512i chunks_n = _mm512_or_si512(lo_n, hi_n);

      // Step C: Extract next8 chunk (R >> k)
      chunk = 0;
      if(word_idx < NUM_WORDS)
      {
        chunk = r[word_idx] >> bit_shift;
      }
      if(word_idx + 1 < NUM_WORDS && bit_shift != 0)
      {
        // Cast to uint64_t before shifting to prevent any 32-bit boundary overflow
        chunk |= ((uint64_t)r[word_idx + 1] << (32 - bit_shift));
      }
      next_8 = (uint32_t)chunk;

      // Step D: Apply Modulo 2^k instantly
      if(word_idx < NUM_WORDS)
      {
        r[word_idx] &= mod_mask;
        std::memset(static_cast<void *>(&r[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);
      }

      rrprime_3 = _mm512_mask_and_epi64(rrprime_3, 0b1000'0000, rrprime_3, mod_vec);

      rrprime_4 = ZERO;

      simdy_8 = _mm_extract_epi32(_mm512_castsi512_si128(chunks_n), 0);

      assert(simdy_8 == next_8);

      // Step E: Print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }

    // std::cout << "\n";
  }

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t POW_5_E[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm512i
    0,          0,          0,          0,          0,          0,          0,          0, // 1 - again
    0,          0,          0,          0,          0,          0,          0,          0, // 2 - again

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm512i
    2259883222, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,       // 2 - again

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3594030316, 3284427401, 4246652165, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -
    0,          0,          0,          0,          0,          0,          0,          0,          // 2 - again

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081, 3378214371, // 0 - _mm256i loaded and 0 extended to a _mm512i
    1653863564, 613940998,  1392057263, 562793371,  894754680,  2060902835, 2039635717, 2771255238, // 1 - again -
    276,        0,          0,          0,          0,          0,          0,          0,          // 2 - again

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648, 3583367183, 649228654,  // 0 - _mm256i loaded and 0 extended to a _mm512i
    2915460784, 487929380,  1011012442, 1677677582, 3428152256, 1710878487, 1438394610, 2161952759, // 1 - again -
    4100910556, 1608314830, 349175,     0,          0,          0,          0,          0,          // 2 - again

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860351, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    234079247,  2101059099, 1948702207, 947446250,  864457656,  507589568,  1321007357, 3911984176, // 1 - again -
    1011110295, 2382358050, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again

    1352440577, 3353128433, 943566124,  1942375735, 3933038544, 1369155590, 3288593144, 2689786274, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3608159034, 1149596585, 3474119098, 1887901699, 1390164635, 3853669096, 119012174,  3192759855, // 1 - again -
    179034081,  2355293519, 3468291195, 3563470881, 2702541568, 3437458918, 2217683442, 129,        // 2 - again
  };

  for(int k = 1050; k < 1075; k++)
  {
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const uint32_t P = std::floor(k * std::log10(2));

    const uint32_t P_3 = P >> 3U;

    const uint32_t P_3_4 = P_3 >> 2U;

    const __m512i ZERO = _mm512_setzero_si512();

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&POW_5_CACHE[POW_5_IDX[P_3_4]]));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&POW_5_CACHE[POW_5_IDX[P_3_4] + 8U]));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_epi32(&POW_5_CACHE[POW_5_IDX[P_3_4] + 16U]));
    __m512i rrprime_4 = ZERO;
    __m512i r1e8 = _mm512_set1_epi64(POW_5_CORRECTION[8]);
    __m512i r1emis = _mm512_set1_epi64(POW_5_CORRECTION[P - (P_3 << 3U)]);

    uint32_t E = POW_5_E[P_3_4];

    for(; E < P_3; E++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }
    {

      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1emis);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1emis);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1emis);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1emis);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      rrprime_1 = _mm512_add_epi32(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi32(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi32(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi32(u32_lo_prod_4, u32_hi_prod_slide_4);
      const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_1, ZERO, 7), 32);
      const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_2, rrprime_1, 7), 32);
      const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_3, rrprime_2, 7), 32);
      const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrprime_4, rrprime_3, 7), 32);
      rrprime_1 = _mm512_add_epi32(rrprime_1, rrprime_slide_1);
      rrprime_2 = _mm512_add_epi32(rrprime_2, rrprime_slide_2);
      rrprime_3 = _mm512_add_epi32(rrprime_3, rrprime_slide_3);
      rrprime_4 = _mm512_add_epi32(rrprime_4, rrprime_slide_4);
    }

    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> written = { 0 };
    std::array<uint32_t, NUM_WORDS> r = { 0 };

    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);

    _mm256_storeu_epi32(static_cast<void *>(&written[0]), u32_pack_1);
    _mm256_storeu_epi32(static_cast<void *>(&written[8]), u32_pack_2);
    _mm256_storeu_epi32(static_cast<void *>(&written[16]), u32_pack_3);
    _mm256_storeu_epi32(static_cast<void *>(&written[24]), u32_pack_4);

    r[0] = 1; // Initialize R = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(int w = 0; w < NUM_WORDS; ++w)
      {
        uint64_t p = (uint64_t)r[w] * 10ULL + carry;
        r[w] = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }
    }

    size_t first = 0;

    while(first < r.size() && r[first] == 0)
    {
      ++first;
    }

    if(first != 0 && first != r.size())
    {
      const size_t count = r.size() - first;

      std::memmove(r.data(), r.data() + first, count * sizeof(uint32_t));

      std::memset(r.data() + count, 0, first * sizeof(uint32_t));
    }

    assert(written == r);
  }

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t POW_5_CACHE[] = {
    1,          0,          0,          0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    2242703233, 762134875,  1262,       0,          0,          0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    3211403009, 1849224548, 3668416493, 3913284084, 1593091,    0,          0,          0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    3776417409, 3833115195, 474402842,  2046101519, 1659368615, 1657637457, 2010764683, 0, // 0 - _mm256i loaded and 0 extended to a _mm512i

    781532673,  64985353,   253049085,  594863151,  3553621484, 3288652808, 3167596762, 2788392729, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3911132675, 590,        0,          0,          0,          0,          0,          0,          // 1 - again -

    4223872385, 1378013428, 3071223888, 1460925171, 3695318035, 3594083709, 4166588825, 9582620, // 0 - _mm256i loaded and 0 extended to a _mm512i
    2259883221, 2057664441, 314048097,  745834,     0,          0,          0,          0,       // 1 - again -

    2894159105, 2850127391, 1678958885, 1990048638, 4055408710, 4272372864, 3869429580, 1940770817, // 0 - _mm256i loaded and 0 extended to a _mm512i
    1351327083, 2522292526, 4246650903, 2708404372, 1313461594, 941374947,  0,          0,          // 1 - again -

    3031420033, 1112378521, 855339782,  4191278750, 248038208,  1136169807, 2230652081,    // 0 - _mm256i loaded and 0 extended to a _mm512i
    3378214371, 2737427851, 3059683745, 2018608065, 944476582,  893161588,  2060902835,    // 1 - again -
    2039635717, 2771255238, 276,        0,          0,          0,          0,          0, // 2 - again -

    2553183233, 3201533787, 3638140786, 303378311,  1809731782, 3477761648,                // 0 - _mm256i loaded and 0 extended to a _mm512i
    3583367183, 649228654,  3434010671, 949781480,  536609599,  3926543359,                // 1 -  again  -
    1768783640, 53241030,   3722597222, 2161952758, 4100910556, 1608314830, 349175,     0, // 2 -  again  -

    3940379521, 184513341,  2872588323, 2214530454, 38258512,   2980860350, 114267010,  2188874685, // 0 - _mm256i loaded and 0 extended to a _mm512i
    3647513870, 2036073745, 1695653122, 352583099,  1605803468, 1513904055, 205674657,  361456571,  // 1 - again  -
    1394943654, 2382357459, 2389730781, 730678769,  440721283,  0,          0,          0,          // 2 - again  -

    1352440577, 651996657,  477028415,  1942321525, 3933038544, 3421419653, 2526458268, 2689785012, // 0 - _mm256i loaded and 0 extended to a _mm512i
    559387833,  2512249609, 3034169254, 426976498,  1989813896, 259585386,  1330954932, 1333952685, // 1 - again  -
    2840668958, 679312288,  3152650006, 3562725047, 1409792,    2970921208, 2217629232, 129,        // 2 - again  -
  };

  std::cout << "{";

  std::vector<bool> done(41, false);

  for(uint32_t k = 1; k < 1074; k++)
  {
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const uint32_t P = std::floor(k * std::log10(2));
    // Initialize our base as a BigInt equivalent to 10^8
    uint32_t E = 0;

    const __m512i ZERO = _mm512_setzero_si512();
    __m512i rrprime_1 = _mm512_setr_epi64(1, 0, 0, 0, 0, 0, 0, 0);
    __m512i rrprime_2 = ZERO;
    __m512i rrprime_3 = ZERO;
    __m512i rrprime_4 = ZERO;
    __m512i rrprime_5 = ZERO;
    __m512i r1e8 = _mm512_set1_epi64(100'000'000U);

    const uint32_t P_3 = P >> 3U;

    uint32_t written[40];

    for(; E < P_3; E++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_lo_prod_5 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_5, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_5 = _mm512_srli_epi64(u64_prod_5, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      const __m512i u32_hi_prod_slide_5 = _mm512_alignr_epi64(u32_hi_prod_5, u32_hi_prod_4, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm512_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
    }

    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
    const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);

    _mm256_storeu_epi32(static_cast<void *>(&written[0]), u32_pack_1);
    _mm256_storeu_epi32(static_cast<void *>(&written[8]), u32_pack_2);
    _mm256_storeu_epi32(static_cast<void *>(&written[16]), u32_pack_3);
    _mm256_storeu_epi32(static_cast<void *>(&written[24]), u32_pack_4);
    _mm256_storeu_epi32(static_cast<void *>(&written[32]), u32_pack_5);

    if(done[P_3] == false && P_3 % 4 == 0)
    {
      done[P_3] = true;
      std::cout << "{ ";
      for(const auto &i : written)
      {
        std::cout << i << ",";
      }
      std::cout << "}, ";
    }
  }

  std::cout << "} \n";

  return 0;
}
*/

/*
int main()
{
  constexpr uint32_t POW_5_CACHE[] = {
    2701131776, 466537709, 54210, 0,                // 0 - 256 bit (8  uint32_t's)
    0, 0, 0, 0,                                     // 1 -
                                                    //
    553648128, 1943292173, 4261398408, 2484038180,  // 0 - 256 bit (8 uint32_t's)
    68422776, 0, 0, 0,                              // 1 -
                                                    //
    2701131776, 807615852, 3882706566, 3057181734,  // 0 - 256 bit (8 uint32_t's)
    745289159, 4056365773, 462339630, 20,           // 1 -
                                                    //
    1581318144, 3996984702, 317337323, 3761442594,  // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    3054408957, 3984113819, 1405123673, 3088262246, // 1 -
    1090037, 0, 0, 0,                               // 2 -
    0, 0, 0, 0,                                     // 3 -
                                                    //
    767623168, 4241160024, 1260959332, 2541775228,  // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    2965753944, 1796720685, 484800439, 1311835347,  // 1 -
    2945126454, 3563705203, 1375821026, 0,          // 2 -
    0, 0, 0, 0,                                     // 3 -
                                                    //
    1027670016, 584120530, 2817733607, 1418065558,  // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    2125434793, 2583803420, 1379713046, 3308183724, // 1 -
    3479371296, 2677438127, 4145998343, 931079930,  // 2 -
    1363485451, 404, 0, 0,                          // 3 -
                                                    //
    2361458688, 1372100044, 4019207324, 1803970613, // 0 - 512 bit (16 uint32_t's) // will be loaded as 2 separate (_mm256i) and 0 extended
    3054822823, 2484332018, 886717433, 979406226,   // 1 -
    178043906, 3605602663, 1325973018, 2766841440,  // 2 -
    3535169275, 170736516, 1638513683, 510320,      // 3 -
                                                    //
    3207749888, 3090445067, 878803004, 3926310579,  // 0 - 768 bit (24 uint32_t's) // will be loaded as 3 separate (_mm256i) and 0 extended
    2511096468, 3211496945, 4229969674, 2801008543, // 1 -
    3731249614, 2522876060, 2243995037, 263064917,  // 2 -
    3209265061, 1558881170, 2972226309, 387950025,  // 3 -
    1894719538, 6, 0, 0,                            // 4 -
    0, 0, 0, 0,                                     // 5 -
                                                    //
    2364662016, 1050217897, 464427981, 3271896570,  // 0 - 768 bit (24 uint32_t's) // will be loaded as 3 separate (_mm256i) and 0 extended
    2431110680, 1321427493, 3373513620, 2398142296, // 1 -
    2095248896, 1505421683, 3598109983, 3281306459, // 2 -
    1545540873, 1451518959, 3120402494, 1029160510, // 3 -
    1905946741, 3130719652, 3748297313, 8129,       // 4 -
    0, 0, 0, 0,                                     // 5 -
                                                    //
    49373440, 69305454, 3524743267, 494625378,      // 0 - 768 bit (24 uint32_t's) // will be loaded as 3 separate (_mm256i) and 0 extended
    1758466655, 1690015127, 707937707, 769832326,   // 1 -
    947974910, 331926644, 3401326036, 4283614656,   // 2 -
    2171178814, 2423608825, 1226070469, 2615635200, // 3 -
    3797307464, 286984290, 2589368102, 3530478015,  // 4 -
    13941208, 10261341, 0, 0                        // 5 -
                                                    //

  };

  std::cout << "{ ";
  for(uint32_t k = 0; k < 1100; k += 100)
  {
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const uint32_t P = std::floor(k * std::log10(2));
    // Initialize our base as a BigInt equivalent to 10^8
    uint32_t E = 0;

    const __m512i ZERO = _mm512_setzero_si512();
    __m512i rrprime_1 = _mm512_setr_epi64(1, 0, 0, 0, 0, 0, 0, 0);
    __m512i rrprime_2 = ZERO;
    __m512i rrprime_3 = ZERO;
    __m512i rrprime_4 = ZERO;
    __m512i rrprime_5 = ZERO;
    __m512i r1e8 = _mm512_set1_epi64(100'000'000U);

    uint32_t written[40];

    for(; E + 8 < P; E += 8)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_lo_prod_5 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_5, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_5 = _mm512_srli_epi64(u64_prod_5, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      const __m512i u32_hi_prod_slide_5 = _mm512_alignr_epi64(u32_hi_prod_5, u32_hi_prod_4, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm512_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
    }

    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
    const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);

    _mm256_storeu_epi32(static_cast<void *>(&written[0]), u32_pack_1);
    _mm256_storeu_epi32(static_cast<void *>(&written[8]), u32_pack_2);
    _mm256_storeu_epi32(static_cast<void *>(&written[16]), u32_pack_3);
    _mm256_storeu_epi32(static_cast<void *>(&written[24]), u32_pack_4);
    _mm256_storeu_epi32(static_cast<void *>(&written[32]), u32_pack_5);

    std::cout << "{ ";
    for(const auto &i : written)
    {
      std::cout << i << ",";
    }
    std::cout << "}, ";

    int alsjdfl = 1;
  }

  std::cout << "}\n";

  return 0;
}
*/

// real algo
/*
int main()
{
  for(uint32_t k = 0; k < 1100; k += 100)
  {
    // 36 words * 32 bits = 1152 bits. Perfectly fits k=1100 + 10^8 overflow.
    const uint32_t P = std::floor(k * std::log10(2));
    // Initialize our base as a BigInt equivalent to 10^8
    uint32_t E = 0;

    const __m512i ZERO = _mm512_setzero_si512();
    __m512i rrprime_1 = _mm512_setr_epi64(1, 0, 0, 0, 0, 0, 0, 0);
    __m512i rrprime_2 = ZERO;
    __m512i rrprime_3 = ZERO;
    __m512i rrprime_4 = ZERO;
    __m512i rrprime_5 = ZERO;
    __m512i r1e8 = _mm512_set1_epi64(100'000'000U);

    for(; E + 8 < P; E += 8)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, r1e8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, r1e8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, r1e8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, r1e8);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, r1e8);
      const __m512i u32_lo_prod_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_1, ZERO);
      const __m512i u32_lo_prod_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_2, ZERO);
      const __m512i u32_lo_prod_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_3, ZERO);
      const __m512i u32_lo_prod_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_4, ZERO);
      const __m512i u32_lo_prod_5 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, u64_prod_5, ZERO);
      const __m512i u32_hi_prod_1 = _mm512_srli_epi64(u64_prod_1, 32);
      const __m512i u32_hi_prod_2 = _mm512_srli_epi64(u64_prod_2, 32);
      const __m512i u32_hi_prod_3 = _mm512_srli_epi64(u64_prod_3, 32);
      const __m512i u32_hi_prod_4 = _mm512_srli_epi64(u64_prod_4, 32);
      const __m512i u32_hi_prod_5 = _mm512_srli_epi64(u64_prod_5, 32);
      const __m512i u32_hi_prod_slide_1 = _mm512_alignr_epi64(u32_hi_prod_1, ZERO, 7);
      const __m512i u32_hi_prod_slide_2 = _mm512_alignr_epi64(u32_hi_prod_2, u32_hi_prod_1, 7);
      const __m512i u32_hi_prod_slide_3 = _mm512_alignr_epi64(u32_hi_prod_3, u32_hi_prod_2, 7);
      const __m512i u32_hi_prod_slide_4 = _mm512_alignr_epi64(u32_hi_prod_4, u32_hi_prod_3, 7);
      const __m512i u32_hi_prod_slide_5 = _mm512_alignr_epi64(u32_hi_prod_5, u32_hi_prod_4, 7);
      rrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
      rrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
      rrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
      rrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
      rrprime_5 = _mm512_add_epi64(u32_lo_prod_5, u32_hi_prod_slide_5);
    }

    int alsjdfl = 1;
  }

  return 0;
}
*/
