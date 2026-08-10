#include <cassert>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <iomanip>
#include <iostream>

#if true || defined(__AVX512BW__) && defined(__AVX512VL__)

int main()
{
  constexpr uint16_t POW_5_E[] = { 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 };
  constexpr uint32_t POW_5_CACHE[] = {
    //  k = 0
    1, 0, 0, 0, 0, 0, 0, 0, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0, // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0, // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0, // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0, // 4 - anotherone

    //  k = 64
    9551616, 67440737, 1844, 0, 0, 0, 0, 0, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0,                 // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                 // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                 // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                 // 4 - anotherone

    //  k = 128
    68211456, 46074317, 46346337, 66920938, 3402823, 0, 0, 0, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 4 - anotherone

    //  k = 192
    34512896, 54444640, 41610235, 23207666, 38357894, 38668076, 77101735, 62, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 4 - anotherone

    //  k = 256
    29639936, 40079131, 3945758, 65640564, 32699846, 68790785, 70985008, 61954235, // __m256i single load cvtepied to __m512i
    8923731, 115792, 0, 0, 0, 0, 0, 0,                                             // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 320
    86936576, 229620, 78064055, 22219725, 70416078, 65276994, 4522356, 21146027, // __m256i single load cvtepied to __m512i
    70616955, 82395021, 59209100, 13598703, 2, 0, 0, 0,                          // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                      // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                      // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                      // 4 - anotherone

    //  k = 384
    90306816, 8066279, 88491564, 14266254, 72106114, 72177149, 93404245, 66679482, // __m256i single load cvtepied to __m512i
    27046544, 5079739, 1436138, 27904010, 94479212, 20061963, 3940, 0,             // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 448
    28614656, 35346016, 26617793, 62521606, 35655607, 5071376, 23326191, 81133979, // __m256i single load cvtepied to __m512i
    18063928, 81490199, 73180602, 64136068, 4534353, 38078880, 89054932, 24295606, // 1 - anotherone
    7268387, 0, 0, 0, 0, 0, 0, 0,                                                  // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 512
    6084096, 64336490, 94656994, 53882811, 60508537, 85818648, 27690031, 81669034,  // __m256i single load cvtepied to __m512i
    80187429, 73546976, 17640300, 56144372, 93377723, 58205923, 12747936, 98205846, // 1 - anotherone
    95740249, 94259709, 7807929, 134, 0, 0, 0, 0,                                   // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 576
    48699136, 66067721, 89316791, 46155101, 12099675, 3430675, 39351365, 60161393, // __m256i single load cvtepied to __m512i
    59767142, 43044989, 64010612, 86717031, 25106531, 60928972, 91406305, 1211839, // 1 - anotherone
    51313491, 64719003, 2521019, 45340605, 40147310, 247330, 0, 0,                 // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 640
    46603776, 28741922, 26495008, 94674360, 41882519, 31972221, 98252661, 13754379, // __m256i single load cvtepied to __m512i
    68870472, 94407310, 23095735, 39977864, 11317371, 65059128, 20867738, 99275167, // 1 - anotherone
    91925178, 55993057, 28507248, 13248932, 60570029, 18641171, 76221952, 56244061, // 2 - anotherone
    4, 0, 0, 0, 0, 0, 0, 0,                                                         // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 704
    72502016, 6920277, 16347634, 20126149, 35710666, 73805511, 99283578, 4300935,   // __m256i single load cvtepied to __m512i
    4503633, 40861810, 9160029, 48340831, 1698851, 96317275, 44141521, 18664580,    // 1 - anotherone
    32931538, 71446875, 94449099, 17519724, 84517078, 48805436, 60820586, 85583812, // 2 - anotherone
    73976115, 21744247, 8416, 0, 0, 0, 0, 0,                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 768
    16057856, 28468538, 46875089, 99193716, 48252629, 63159844, 50265285, 8499055,  // __m256i single load cvtepied to __m512i
    45497602, 81139204, 72750411, 39144428, 44114814, 61715800, 13107320, 87730697, // 1 - anotherone
    64779504, 40882864, 86330878, 26863768, 2605095, 39052038, 66966111, 88601711,  // 2 - anotherone
    2555256, 94884625, 93514897, 92300708, 15525180, 0, 0, 0,                       // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 832
    74295296, 87871064, 58361535, 53606737, 74759305, 23174524, 45040008, 89787762, // __m256i single load cvtepied to __m512i
    80126147, 12102266, 43079792, 62051287, 41564579, 4450260, 7337670, 96860757,   // 1 - anotherone
    97200743, 50921899, 75429359, 58246283, 34528426, 52904159, 10640053, 44970231, // 2 - anotherone
    52529441, 91721706, 49072173, 33674838, 44187839, 47496120, 38903918, 286,      // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 896
    37998336, 85808977, 47648953, 98767036, 88383968, 75156173, 48028292, 11884041, // __m256i single load cvtepied to __m512i
    23290821, 41053024, 76764264, 16873151, 99005084, 83866839, 93957690, 38978462, // 1 - anotherone
    55372505, 55950268, 78882347, 32571286, 67439499, 57187894, 72558876, 85947441, // 2 - anotherone
    2556724, 90266739, 12796070, 36121522, 88473260, 51660651, 39784916, 52463523,  // 3 - anotherone
    53113566, 528294, 0, 0, 0, 0, 0, 0,                                             // 4 - anotherone

    //  k = 960
    14110976, 85899919, 54708182, 3104277, 28277388, 46896561, 46363615, 48749457, // __m256i single load cvtepied to __m512i
    59792539, 78873685, 34792183, 35279959, 90053648, 7896554, 47302987, 77699956, // 1 - anotherone
    34195312, 29631265, 46577987, 52030940, 45916986, 8668183, 49162314, 25922530, // 2 - anotherone
    65882010, 24656150, 76642610, 12948690, 79064572, 752686, 26857595, 83108762,  // 3 - anotherone
    38787518, 80353382, 13999990, 74531401, 9, 0, 0, 0                             // 4 - anotherone
  };

  for(int k = 0; k < 1023; k++)
  {
  backwards:
    constexpr unsigned NUM_WORDS = 40;

    const unsigned K_DIV_64 = static_cast<unsigned>(k) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 40;
    const unsigned E_0 = POW_5_E[K_DIV_64];

    const __m512i ZERO = _mm512_setzero_si512();

    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 96> SIMD_ARRAY;

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX])));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 8])));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 16])));
    __m512i rrprime_4 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 24])));
    __m512i rrprime_5 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 32])));

    const __m512i R_10E8 = _mm512_set1_epi64(100'000'000U);
    const __m512i R_MAGIC_10E8 = _mm512_set1_epi64(1'441'151'881U);

    {
      int e = E_0;
      for(; e + 5 < k; e += 5) // each iteration has ~25 cycle latency --- since 8 iterations is worst case then 200 cycle latency just in this loop ...
      {
        const __m512i u64_prod_1 = _mm512_slli_epi64(rrprime_1, 5);
        const __m512i u64_prod_2 = _mm512_slli_epi64(rrprime_2, 5);
        const __m512i u64_prod_3 = _mm512_slli_epi64(rrprime_3, 5);
        const __m512i u64_prod_4 = _mm512_slli_epi64(rrprime_4, 5);
        const __m512i u64_prod_5 = _mm512_slli_epi64(rrprime_5, 5);
        const __m512i u64_magic_prod_1 = _mm512_mul_epu32(u64_prod_1, R_MAGIC_10E8);
        const __m512i u64_magic_prod_2 = _mm512_mul_epu32(u64_prod_2, R_MAGIC_10E8);
        const __m512i u64_magic_prod_3 = _mm512_mul_epu32(u64_prod_3, R_MAGIC_10E8);
        const __m512i u64_magic_prod_4 = _mm512_mul_epu32(u64_prod_4, R_MAGIC_10E8);
        const __m512i u64_magic_prod_5 = _mm512_mul_epu32(u64_prod_5, R_MAGIC_10E8);
        const __m512i u64_magic_res_1 = _mm512_srli_epi64(u64_magic_prod_1, 57U);
        const __m512i u64_magic_res_2 = _mm512_srli_epi64(u64_magic_prod_2, 57U);
        const __m512i u64_magic_res_3 = _mm512_srli_epi64(u64_magic_prod_3, 57U);
        const __m512i u64_magic_res_4 = _mm512_srli_epi64(u64_magic_prod_4, 57U);
        const __m512i u64_magic_res_5 = _mm512_srli_epi64(u64_magic_prod_5, 57U);
        const __m512i u64_magic_res_x10E8_1 = _mm512_mul_epu32(u64_magic_res_1, R_10E8);
        const __m512i u64_magic_res_x10E8_2 = _mm512_mul_epu32(u64_magic_res_2, R_10E8);
        const __m512i u64_magic_res_x10E8_3 = _mm512_mul_epu32(u64_magic_res_3, R_10E8);
        const __m512i u64_magic_res_x10E8_4 = _mm512_mul_epu32(u64_magic_res_4, R_10E8);
        const __m512i u64_magic_res_x10E8_5 = _mm512_mul_epu32(u64_magic_res_5, R_10E8);
        const __m512i u64_magic_carry_slide_1 = _mm512_alignr_epi64(u64_magic_res_1, ZERO, 7);
        const __m512i u64_magic_carry_slide_2 = _mm512_alignr_epi64(u64_magic_res_2, u64_magic_res_1, 7);
        const __m512i u64_magic_carry_slide_3 = _mm512_alignr_epi64(u64_magic_res_3, u64_magic_res_2, 7);
        const __m512i u64_magic_carry_slide_4 = _mm512_alignr_epi64(u64_magic_res_4, u64_magic_res_3, 7);
        const __m512i u64_magic_carry_slide_5 = _mm512_alignr_epi64(u64_magic_res_5, u64_magic_res_4, 7);
        rrprime_1 = _mm512_add_epi64(u64_prod_1, u64_magic_carry_slide_1);
        rrprime_2 = _mm512_add_epi64(u64_prod_2, u64_magic_carry_slide_2);
        rrprime_3 = _mm512_add_epi64(u64_prod_3, u64_magic_carry_slide_3);
        rrprime_4 = _mm512_add_epi64(u64_prod_4, u64_magic_carry_slide_4);
        rrprime_5 = _mm512_add_epi64(u64_prod_5, u64_magic_carry_slide_5);

        rrprime_1 = _mm512_sub_epi32(rrprime_1, u64_magic_res_x10E8_1);
        rrprime_2 = _mm512_sub_epi32(rrprime_2, u64_magic_res_x10E8_2);
        rrprime_3 = _mm512_sub_epi32(rrprime_3, u64_magic_res_x10E8_3);
        rrprime_4 = _mm512_sub_epi32(rrprime_4, u64_magic_res_x10E8_4);
        rrprime_5 = _mm512_sub_epi32(rrprime_5, u64_magic_res_x10E8_5);
      }

      int missing = k - e;

      if(missing)
      {
        const __m512i u64_prod_1 = _mm512_slli_epi64(rrprime_1, 5);
        const __m512i u64_prod_2 = _mm512_slli_epi64(rrprime_2, 5);
        const __m512i u64_prod_3 = _mm512_slli_epi64(rrprime_3, 5);
        const __m512i u64_prod_4 = _mm512_slli_epi64(rrprime_4, 5);
        const __m512i u64_prod_5 = _mm512_slli_epi64(rrprime_5, 5);
        const __m512i u64_magic_prod_1 = _mm512_mul_epu32(u64_prod_1, R_MAGIC_10E8);
        const __m512i u64_magic_prod_2 = _mm512_mul_epu32(u64_prod_2, R_MAGIC_10E8);
        const __m512i u64_magic_prod_3 = _mm512_mul_epu32(u64_prod_3, R_MAGIC_10E8);
        const __m512i u64_magic_prod_4 = _mm512_mul_epu32(u64_prod_4, R_MAGIC_10E8);
        const __m512i u64_magic_prod_5 = _mm512_mul_epu32(u64_prod_5, R_MAGIC_10E8);
        const __m512i u64_magic_res_1 = _mm512_srli_epi64(u64_magic_prod_1, 57U);
        const __m512i u64_magic_res_2 = _mm512_srli_epi64(u64_magic_prod_2, 57U);
        const __m512i u64_magic_res_3 = _mm512_srli_epi64(u64_magic_prod_3, 57U);
        const __m512i u64_magic_res_4 = _mm512_srli_epi64(u64_magic_prod_4, 57U);
        const __m512i u64_magic_res_5 = _mm512_srli_epi64(u64_magic_prod_5, 57U);
        const __m512i u64_magic_res_x10E8_1 = _mm512_mul_epu32(u64_magic_res_1, R_10E8);
        const __m512i u64_magic_res_x10E8_2 = _mm512_mul_epu32(u64_magic_res_2, R_10E8);
        const __m512i u64_magic_res_x10E8_3 = _mm512_mul_epu32(u64_magic_res_3, R_10E8);
        const __m512i u64_magic_res_x10E8_4 = _mm512_mul_epu32(u64_magic_res_4, R_10E8);
        const __m512i u64_magic_res_x10E8_5 = _mm512_mul_epu32(u64_magic_res_5, R_10E8);
        const __m512i u64_magic_carry_slide_1 = _mm512_alignr_epi64(u64_magic_res_1, ZERO, 7);
        const __m512i u64_magic_carry_slide_2 = _mm512_alignr_epi64(u64_magic_res_2, u64_magic_res_1, 7);
        const __m512i u64_magic_carry_slide_3 = _mm512_alignr_epi64(u64_magic_res_3, u64_magic_res_2, 7);
        const __m512i u64_magic_carry_slide_4 = _mm512_alignr_epi64(u64_magic_res_4, u64_magic_res_3, 7);
        const __m512i u64_magic_carry_slide_5 = _mm512_alignr_epi64(u64_magic_res_5, u64_magic_res_4, 7);
        rrprime_1 = _mm512_add_epi64(u64_prod_1, u64_magic_carry_slide_1);
        rrprime_2 = _mm512_add_epi64(u64_prod_2, u64_magic_carry_slide_2);
        rrprime_3 = _mm512_add_epi64(u64_prod_3, u64_magic_carry_slide_3);
        rrprime_4 = _mm512_add_epi64(u64_prod_4, u64_magic_carry_slide_4);
        rrprime_5 = _mm512_add_epi64(u64_prod_5, u64_magic_carry_slide_5);

        rrprime_1 = _mm512_sub_epi32(rrprime_1, u64_magic_res_x10E8_1);
        rrprime_2 = _mm512_sub_epi32(rrprime_2, u64_magic_res_x10E8_2);
        rrprime_3 = _mm512_sub_epi32(rrprime_3, u64_magic_res_x10E8_3);
        rrprime_4 = _mm512_sub_epi32(rrprime_4, u64_magic_res_x10E8_4);
        rrprime_5 = _mm512_sub_epi32(rrprime_5, u64_magic_res_x10E8_5);
      }
    }

    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), rrprime_2);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), rrprime_3);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), rrprime_4);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[32]), rrprime_5);

    {
      const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
      const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
      const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
      const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
      const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);

      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_2);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_3);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_4);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[32]), u32_pack_5);
    }

    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    SLOW_PRE_ARR[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 2 in each iteration)
    unsigned i;
    for(i = 0; i + 5 < k; i += 5)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = ((uint64_t)w << 5U) + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    const unsigned miss = k - i;
    uint32_t carry = 0;
    for(unsigned int &w : SLOW_PRE_ARR)
    {
      uint64_t pp = ((uint64_t)w << miss) + carry;
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = (uint32_t)(pp - carry * 100'000'000U);
    }

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
      std::cout << "k = " << k << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    //
  }

  return 0;
}

#elif defined(__AVX2__)

int main()
{
  constexpr uint16_t POW_5_E[] = { 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 };
  constexpr uint32_t POW_5_CACHE[] = {
    //  k = 0
    1, 0, 0, 0, 0, 0, 0, 0, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0, // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0, // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0, // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0, // 4 - anotherone

    //  k = 64
    9551616, 67440737, 1844, 0, 0, 0, 0, 0, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0,                 // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                 // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                 // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                 // 4 - anotherone

    //  k = 128
    68211456, 46074317, 46346337, 66920938, 3402823, 0, 0, 0, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                   // 4 - anotherone

    //  k = 192
    34512896, 54444640, 41610235, 23207666, 38357894, 38668076, 77101735, 62, // __m256i single load cvtepied to __m512i
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                   // 4 - anotherone

    //  k = 256
    29639936, 40079131, 3945758, 65640564, 32699846, 68790785, 70985008, 61954235, // __m256i single load cvtepied to __m512i
    8923731, 115792, 0, 0, 0, 0, 0, 0,                                             // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 320
    86936576, 229620, 78064055, 22219725, 70416078, 65276994, 4522356, 21146027, // __m256i single load cvtepied to __m512i
    70616955, 82395021, 59209100, 13598703, 2, 0, 0, 0,                          // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                      // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                      // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                      // 4 - anotherone

    //  k = 384
    90306816, 8066279, 88491564, 14266254, 72106114, 72177149, 93404245, 66679482, // __m256i single load cvtepied to __m512i
    27046544, 5079739, 1436138, 27904010, 94479212, 20061963, 3940, 0,             // 1 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 448
    28614656, 35346016, 26617793, 62521606, 35655607, 5071376, 23326191, 81133979, // __m256i single load cvtepied to __m512i
    18063928, 81490199, 73180602, 64136068, 4534353, 38078880, 89054932, 24295606, // 1 - anotherone
    7268387, 0, 0, 0, 0, 0, 0, 0,                                                  // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 512
    6084096, 64336490, 94656994, 53882811, 60508537, 85818648, 27690031, 81669034,  // __m256i single load cvtepied to __m512i
    80187429, 73546976, 17640300, 56144372, 93377723, 58205923, 12747936, 98205846, // 1 - anotherone
    95740249, 94259709, 7807929, 134, 0, 0, 0, 0,                                   // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 576
    48699136, 66067721, 89316791, 46155101, 12099675, 3430675, 39351365, 60161393, // __m256i single load cvtepied to __m512i
    59767142, 43044989, 64010612, 86717031, 25106531, 60928972, 91406305, 1211839, // 1 - anotherone
    51313491, 64719003, 2521019, 45340605, 40147310, 247330, 0, 0,                 // 2 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone

    //  k = 640
    46603776, 28741922, 26495008, 94674360, 41882519, 31972221, 98252661, 13754379, // __m256i single load cvtepied to __m512i
    68870472, 94407310, 23095735, 39977864, 11317371, 65059128, 20867738, 99275167, // 1 - anotherone
    91925178, 55993057, 28507248, 13248932, 60570029, 18641171, 76221952, 56244061, // 2 - anotherone
    4, 0, 0, 0, 0, 0, 0, 0,                                                         // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 704
    72502016, 6920277, 16347634, 20126149, 35710666, 73805511, 99283578, 4300935,   // __m256i single load cvtepied to __m512i
    4503633, 40861810, 9160029, 48340831, 1698851, 96317275, 44141521, 18664580,    // 1 - anotherone
    32931538, 71446875, 94449099, 17519724, 84517078, 48805436, 60820586, 85583812, // 2 - anotherone
    73976115, 21744247, 8416, 0, 0, 0, 0, 0,                                        // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 768
    16057856, 28468538, 46875089, 99193716, 48252629, 63159844, 50265285, 8499055,  // __m256i single load cvtepied to __m512i
    45497602, 81139204, 72750411, 39144428, 44114814, 61715800, 13107320, 87730697, // 1 - anotherone
    64779504, 40882864, 86330878, 26863768, 2605095, 39052038, 66966111, 88601711,  // 2 - anotherone
    2555256, 94884625, 93514897, 92300708, 15525180, 0, 0, 0,                       // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 832
    74295296, 87871064, 58361535, 53606737, 74759305, 23174524, 45040008, 89787762, // __m256i single load cvtepied to __m512i
    80126147, 12102266, 43079792, 62051287, 41564579, 4450260, 7337670, 96860757,   // 1 - anotherone
    97200743, 50921899, 75429359, 58246283, 34528426, 52904159, 10640053, 44970231, // 2 - anotherone
    52529441, 91721706, 49072173, 33674838, 44187839, 47496120, 38903918, 286,      // 3 - anotherone
    0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone

    //  k = 896
    37998336, 85808977, 47648953, 98767036, 88383968, 75156173, 48028292, 11884041, // __m256i single load cvtepied to __m512i
    23290821, 41053024, 76764264, 16873151, 99005084, 83866839, 93957690, 38978462, // 1 - anotherone
    55372505, 55950268, 78882347, 32571286, 67439499, 57187894, 72558876, 85947441, // 2 - anotherone
    2556724, 90266739, 12796070, 36121522, 88473260, 51660651, 39784916, 52463523,  // 3 - anotherone
    53113566, 528294, 0, 0, 0, 0, 0, 0,                                             // 4 - anotherone

    //  k = 960
    14110976, 85899919, 54708182, 3104277, 28277388, 46896561, 46363615, 48749457, // __m256i single load cvtepied to __m512i
    59792539, 78873685, 34792183, 35279959, 90053648, 7896554, 47302987, 77699956, // 1 - anotherone
    34195312, 29631265, 46577987, 52030940, 45916986, 8668183, 49162314, 25922530, // 2 - anotherone
    65882010, 24656150, 76642610, 12948690, 79064572, 752686, 26857595, 83108762,  // 3 - anotherone
    38787518, 80353382, 13999990, 74531401, 9, 0, 0, 0                             // 4 - anotherone
  };

  for(int k = 0; k < 1023; k++)
  {
  backwards:
    constexpr unsigned NUM_WORDS = 40;

    const unsigned K_DIV_64 = static_cast<unsigned>(k) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 40;
    const unsigned E_0 = POW_5_E[K_DIV_64];

    const __m256i ZERO = _mm256_setzero_si256();

    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 96> SIMD_ARRAY;

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 4])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 8])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 12])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 16])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 20])));
    __m256i rrprime_7 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 24])));
    __m256i rrprime_8 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 28])));
    __m256i rrprime_9 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 32])));
    __m256i rrprime_10 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 36])));

    const __m256i R_10E8 = _mm256_set1_epi64x(100'000'000U);
    const __m256i R_MAGIC_10E8 = _mm256_set1_epi64x(1'441'151'881U);

    {
      int e = E_0;
      for(; e + 5 < k; e += 5) // each iteration has ~25 cycle latency --- since 8 iterations is worst case then 200 cycle latency just in this loop ...
      {
        const __m256i u64_prod_1 = _mm256_slli_epi64(rrprime_1, 5);
        const __m256i u64_prod_2 = _mm256_slli_epi64(rrprime_2, 5);
        const __m256i u64_prod_3 = _mm256_slli_epi64(rrprime_3, 5);
        const __m256i u64_prod_4 = _mm256_slli_epi64(rrprime_4, 5);
        const __m256i u64_prod_5 = _mm256_slli_epi64(rrprime_5, 5);
        const __m256i u64_prod_6 = _mm256_slli_epi64(rrprime_6, 5);
        const __m256i u64_prod_7 = _mm256_slli_epi64(rrprime_7, 5);
        const __m256i u64_prod_8 = _mm256_slli_epi64(rrprime_8, 5);
        const __m256i u64_prod_9 = _mm256_slli_epi64(rrprime_9, 5);
        const __m256i u64_prod_10 = _mm256_slli_epi64(rrprime_10, 5);
        const __m256i u64_magic_prod_1 = _mm256_mul_epu32(u64_prod_1, R_MAGIC_10E8);
        const __m256i u64_magic_prod_2 = _mm256_mul_epu32(u64_prod_2, R_MAGIC_10E8);
        const __m256i u64_magic_prod_3 = _mm256_mul_epu32(u64_prod_3, R_MAGIC_10E8);
        const __m256i u64_magic_prod_4 = _mm256_mul_epu32(u64_prod_4, R_MAGIC_10E8);
        const __m256i u64_magic_prod_5 = _mm256_mul_epu32(u64_prod_5, R_MAGIC_10E8);
        const __m256i u64_magic_prod_6 = _mm256_mul_epu32(u64_prod_6, R_MAGIC_10E8);
        const __m256i u64_magic_prod_7 = _mm256_mul_epu32(u64_prod_7, R_MAGIC_10E8);
        const __m256i u64_magic_prod_8 = _mm256_mul_epu32(u64_prod_8, R_MAGIC_10E8);
        const __m256i u64_magic_prod_9 = _mm256_mul_epu32(u64_prod_9, R_MAGIC_10E8);
        const __m256i u64_magic_prod_10 = _mm256_mul_epu32(u64_prod_10, R_MAGIC_10E8);
        const __m256i u64_magic_res_1 = _mm256_srli_epi64(u64_magic_prod_1, 57U);
        const __m256i u64_magic_res_2 = _mm256_srli_epi64(u64_magic_prod_2, 57U);
        const __m256i u64_magic_res_3 = _mm256_srli_epi64(u64_magic_prod_3, 57U);
        const __m256i u64_magic_res_4 = _mm256_srli_epi64(u64_magic_prod_4, 57U);
        const __m256i u64_magic_res_5 = _mm256_srli_epi64(u64_magic_prod_5, 57U);
        const __m256i u64_magic_res_6 = _mm256_srli_epi64(u64_magic_prod_6, 57U);
        const __m256i u64_magic_res_7 = _mm256_srli_epi64(u64_magic_prod_7, 57U);
        const __m256i u64_magic_res_8 = _mm256_srli_epi64(u64_magic_prod_8, 57U);
        const __m256i u64_magic_res_9 = _mm256_srli_epi64(u64_magic_prod_9, 57U);
        const __m256i u64_magic_res_10 = _mm256_srli_epi64(u64_magic_prod_10, 57U);
        const __m256i u64_magic_res_x10E8_1 = _mm256_mul_epu32(u64_magic_res_1, R_10E8);
        const __m256i u64_magic_res_x10E8_2 = _mm256_mul_epu32(u64_magic_res_2, R_10E8);
        const __m256i u64_magic_res_x10E8_3 = _mm256_mul_epu32(u64_magic_res_3, R_10E8);
        const __m256i u64_magic_res_x10E8_4 = _mm256_mul_epu32(u64_magic_res_4, R_10E8);
        const __m256i u64_magic_res_x10E8_5 = _mm256_mul_epu32(u64_magic_res_5, R_10E8);
        const __m256i u64_magic_res_x10E8_6 = _mm256_mul_epu32(u64_magic_res_6, R_10E8);
        const __m256i u64_magic_res_x10E8_7 = _mm256_mul_epu32(u64_magic_res_7, R_10E8);
        const __m256i u64_magic_res_x10E8_8 = _mm256_mul_epu32(u64_magic_res_8, R_10E8);
        const __m256i u64_magic_res_x10E8_9 = _mm256_mul_epu32(u64_magic_res_9, R_10E8);
        const __m256i u64_magic_res_x10E8_10 = _mm256_mul_epu32(u64_magic_res_10, R_10E8);

        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u64_magic_res_1, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u64_magic_res_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u64_magic_res_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u64_magic_res_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u64_magic_res_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u64_magic_res_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u64_magic_res_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u64_magic_res_8, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_9 = _mm256_permute4x64_epi64(u64_magic_res_9, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_10 = _mm256_permute4x64_epi64(u64_magic_res_10, 0b10'01'00'11);
        const __m256i u64_magic_carry_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_9 = _mm256_blend_epi32(u32_hi_prod_perm_9, u32_hi_prod_perm_8, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_10 = _mm256_blend_epi32(u32_hi_prod_perm_10, u32_hi_prod_perm_9, 0b00'00'00'11);

        rrprime_1 = _mm256_add_epi64(u64_prod_1, u64_magic_carry_slide_1);
        rrprime_2 = _mm256_add_epi64(u64_prod_2, u64_magic_carry_slide_2);
        rrprime_3 = _mm256_add_epi64(u64_prod_3, u64_magic_carry_slide_3);
        rrprime_4 = _mm256_add_epi64(u64_prod_4, u64_magic_carry_slide_4);
        rrprime_5 = _mm256_add_epi64(u64_prod_5, u64_magic_carry_slide_5);
        rrprime_6 = _mm256_add_epi64(u64_prod_6, u64_magic_carry_slide_6);
        rrprime_7 = _mm256_add_epi64(u64_prod_7, u64_magic_carry_slide_7);
        rrprime_8 = _mm256_add_epi64(u64_prod_8, u64_magic_carry_slide_8);
        rrprime_9 = _mm256_add_epi64(u64_prod_9, u64_magic_carry_slide_9);
        rrprime_10 = _mm256_add_epi64(u64_prod_10, u64_magic_carry_slide_10);

        rrprime_1 = _mm256_sub_epi32(rrprime_1, u64_magic_res_x10E8_1);
        rrprime_2 = _mm256_sub_epi32(rrprime_2, u64_magic_res_x10E8_2);
        rrprime_3 = _mm256_sub_epi32(rrprime_3, u64_magic_res_x10E8_3);
        rrprime_4 = _mm256_sub_epi32(rrprime_4, u64_magic_res_x10E8_4);
        rrprime_5 = _mm256_sub_epi32(rrprime_5, u64_magic_res_x10E8_5);
        rrprime_6 = _mm256_sub_epi32(rrprime_6, u64_magic_res_x10E8_6);
        rrprime_7 = _mm256_sub_epi32(rrprime_7, u64_magic_res_x10E8_7);
        rrprime_8 = _mm256_sub_epi32(rrprime_8, u64_magic_res_x10E8_8);
        rrprime_9 = _mm256_sub_epi32(rrprime_9, u64_magic_res_x10E8_9);
        rrprime_10 = _mm256_sub_epi32(rrprime_10, u64_magic_res_x10E8_10);
      }
      int missing = k - e;

      if(missing)
      {
        const __m256i u64_prod_1 = _mm256_slli_epi64(rrprime_1, missing);
        const __m256i u64_prod_2 = _mm256_slli_epi64(rrprime_2, missing);
        const __m256i u64_prod_3 = _mm256_slli_epi64(rrprime_3, missing);
        const __m256i u64_prod_4 = _mm256_slli_epi64(rrprime_4, missing);
        const __m256i u64_prod_5 = _mm256_slli_epi64(rrprime_5, missing);
        const __m256i u64_prod_6 = _mm256_slli_epi64(rrprime_6, missing);
        const __m256i u64_prod_7 = _mm256_slli_epi64(rrprime_7, missing);
        const __m256i u64_prod_8 = _mm256_slli_epi64(rrprime_8, missing);
        const __m256i u64_prod_9 = _mm256_slli_epi64(rrprime_9, missing);
        const __m256i u64_prod_10 = _mm256_slli_epi64(rrprime_10, missing);
        const __m256i u64_magic_prod_1 = _mm256_mul_epu32(u64_prod_1, R_MAGIC_10E8);
        const __m256i u64_magic_prod_2 = _mm256_mul_epu32(u64_prod_2, R_MAGIC_10E8);
        const __m256i u64_magic_prod_3 = _mm256_mul_epu32(u64_prod_3, R_MAGIC_10E8);
        const __m256i u64_magic_prod_4 = _mm256_mul_epu32(u64_prod_4, R_MAGIC_10E8);
        const __m256i u64_magic_prod_5 = _mm256_mul_epu32(u64_prod_5, R_MAGIC_10E8);
        const __m256i u64_magic_prod_6 = _mm256_mul_epu32(u64_prod_6, R_MAGIC_10E8);
        const __m256i u64_magic_prod_7 = _mm256_mul_epu32(u64_prod_7, R_MAGIC_10E8);
        const __m256i u64_magic_prod_8 = _mm256_mul_epu32(u64_prod_8, R_MAGIC_10E8);
        const __m256i u64_magic_prod_9 = _mm256_mul_epu32(u64_prod_9, R_MAGIC_10E8);
        const __m256i u64_magic_prod_10 = _mm256_mul_epu32(u64_prod_10, R_MAGIC_10E8);
        const __m256i u64_magic_res_1 = _mm256_srli_epi64(u64_magic_prod_1, 57U);
        const __m256i u64_magic_res_2 = _mm256_srli_epi64(u64_magic_prod_2, 57U);
        const __m256i u64_magic_res_3 = _mm256_srli_epi64(u64_magic_prod_3, 57U);
        const __m256i u64_magic_res_4 = _mm256_srli_epi64(u64_magic_prod_4, 57U);
        const __m256i u64_magic_res_5 = _mm256_srli_epi64(u64_magic_prod_5, 57U);
        const __m256i u64_magic_res_6 = _mm256_srli_epi64(u64_magic_prod_6, 57U);
        const __m256i u64_magic_res_7 = _mm256_srli_epi64(u64_magic_prod_7, 57U);
        const __m256i u64_magic_res_8 = _mm256_srli_epi64(u64_magic_prod_8, 57U);
        const __m256i u64_magic_res_9 = _mm256_srli_epi64(u64_magic_prod_9, 57U);
        const __m256i u64_magic_res_10 = _mm256_srli_epi64(u64_magic_prod_10, 57U);
        const __m256i u64_magic_res_x10E8_1 = _mm256_mul_epu32(u64_magic_res_1, R_10E8);
        const __m256i u64_magic_res_x10E8_2 = _mm256_mul_epu32(u64_magic_res_2, R_10E8);
        const __m256i u64_magic_res_x10E8_3 = _mm256_mul_epu32(u64_magic_res_3, R_10E8);
        const __m256i u64_magic_res_x10E8_4 = _mm256_mul_epu32(u64_magic_res_4, R_10E8);
        const __m256i u64_magic_res_x10E8_5 = _mm256_mul_epu32(u64_magic_res_5, R_10E8);
        const __m256i u64_magic_res_x10E8_6 = _mm256_mul_epu32(u64_magic_res_6, R_10E8);
        const __m256i u64_magic_res_x10E8_7 = _mm256_mul_epu32(u64_magic_res_7, R_10E8);
        const __m256i u64_magic_res_x10E8_8 = _mm256_mul_epu32(u64_magic_res_8, R_10E8);
        const __m256i u64_magic_res_x10E8_9 = _mm256_mul_epu32(u64_magic_res_9, R_10E8);
        const __m256i u64_magic_res_x10E8_10 = _mm256_mul_epu32(u64_magic_res_10, R_10E8);

        const __m256i u32_hi_prod_perm_1 = _mm256_permute4x64_epi64(u64_magic_res_1, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_2 = _mm256_permute4x64_epi64(u64_magic_res_2, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_3 = _mm256_permute4x64_epi64(u64_magic_res_3, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_4 = _mm256_permute4x64_epi64(u64_magic_res_4, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_5 = _mm256_permute4x64_epi64(u64_magic_res_5, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_6 = _mm256_permute4x64_epi64(u64_magic_res_6, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_7 = _mm256_permute4x64_epi64(u64_magic_res_7, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_8 = _mm256_permute4x64_epi64(u64_magic_res_8, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_9 = _mm256_permute4x64_epi64(u64_magic_res_9, 0b10'01'00'11);
        const __m256i u32_hi_prod_perm_10 = _mm256_permute4x64_epi64(u64_magic_res_10, 0b10'01'00'11);
        const __m256i u64_magic_carry_slide_1 = _mm256_blend_epi32(u32_hi_prod_perm_1, ZERO, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_2 = _mm256_blend_epi32(u32_hi_prod_perm_2, u32_hi_prod_perm_1, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_3 = _mm256_blend_epi32(u32_hi_prod_perm_3, u32_hi_prod_perm_2, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_4 = _mm256_blend_epi32(u32_hi_prod_perm_4, u32_hi_prod_perm_3, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_5 = _mm256_blend_epi32(u32_hi_prod_perm_5, u32_hi_prod_perm_4, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_6 = _mm256_blend_epi32(u32_hi_prod_perm_6, u32_hi_prod_perm_5, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_7 = _mm256_blend_epi32(u32_hi_prod_perm_7, u32_hi_prod_perm_6, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_8 = _mm256_blend_epi32(u32_hi_prod_perm_8, u32_hi_prod_perm_7, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_9 = _mm256_blend_epi32(u32_hi_prod_perm_9, u32_hi_prod_perm_8, 0b00'00'00'11);
        const __m256i u64_magic_carry_slide_10 = _mm256_blend_epi32(u32_hi_prod_perm_10, u32_hi_prod_perm_9, 0b00'00'00'11);

        rrprime_1 = _mm256_add_epi64(u64_prod_1, u64_magic_carry_slide_1);
        rrprime_2 = _mm256_add_epi64(u64_prod_2, u64_magic_carry_slide_2);
        rrprime_3 = _mm256_add_epi64(u64_prod_3, u64_magic_carry_slide_3);
        rrprime_4 = _mm256_add_epi64(u64_prod_4, u64_magic_carry_slide_4);
        rrprime_5 = _mm256_add_epi64(u64_prod_5, u64_magic_carry_slide_5);
        rrprime_6 = _mm256_add_epi64(u64_prod_6, u64_magic_carry_slide_6);
        rrprime_7 = _mm256_add_epi64(u64_prod_7, u64_magic_carry_slide_7);
        rrprime_8 = _mm256_add_epi64(u64_prod_8, u64_magic_carry_slide_8);
        rrprime_9 = _mm256_add_epi64(u64_prod_9, u64_magic_carry_slide_9);
        rrprime_10 = _mm256_add_epi64(u64_prod_10, u64_magic_carry_slide_10);

        rrprime_1 = _mm256_sub_epi32(rrprime_1, u64_magic_res_x10E8_1);
        rrprime_2 = _mm256_sub_epi32(rrprime_2, u64_magic_res_x10E8_2);
        rrprime_3 = _mm256_sub_epi32(rrprime_3, u64_magic_res_x10E8_3);
        rrprime_4 = _mm256_sub_epi32(rrprime_4, u64_magic_res_x10E8_4);
        rrprime_5 = _mm256_sub_epi32(rrprime_5, u64_magic_res_x10E8_5);
        rrprime_6 = _mm256_sub_epi32(rrprime_6, u64_magic_res_x10E8_6);
        rrprime_7 = _mm256_sub_epi32(rrprime_7, u64_magic_res_x10E8_7);
        rrprime_8 = _mm256_sub_epi32(rrprime_8, u64_magic_res_x10E8_8);
        rrprime_9 = _mm256_sub_epi32(rrprime_9, u64_magic_res_x10E8_9);
        rrprime_10 = _mm256_sub_epi32(rrprime_10, u64_magic_res_x10E8_10);
      }
    }

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), rrprime_2);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), rrprime_3);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), rrprime_4);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), rrprime_5);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), rrprime_6);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), rrprime_7);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[28]), rrprime_8);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[32]), rrprime_9);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[36]), rrprime_10);

    {
      const __m128i u32_pack_1 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_1, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_2 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_2, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_3 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_3, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_4 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_4, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_5 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_5, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_6 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_6, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_7 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_7, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_8 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_8, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_9 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_9, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
      const __m128i u32_pack_10 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_10, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[4]), u32_pack_2);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_3);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[12]), u32_pack_4);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_5);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[20]), u32_pack_6);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_7);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[28]), u32_pack_8);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[32]), u32_pack_9);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_PRE_WRITTEN[36]), u32_pack_10);
    }

    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    SLOW_PRE_ARR[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 2 in each iteration)
    unsigned i;
    for(i = 0; i + 5 < k; i += 5)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = ((uint64_t)w << 5U) + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    const unsigned miss = k - i;
    uint32_t carry = 0;
    for(unsigned int &w : SLOW_PRE_ARR)
    {
      uint64_t pp = ((uint64_t)w << miss) + carry;
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = (uint32_t)(pp - carry * 100'000'000U);
    }

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
      std::cout << "k = " << k << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    //
  }

  return 0;
}

#else
#error "this algorithm is not supported for this architecture; this architecture is too old (pre __AVX2__)"
#endif

int printer_arr()
{
  std::cout << " constexpr uint8_t POW_5_E[] = {";
  for(unsigned k = 0; k < 1024; k++)
  {
    if(k % 64 != 0)
    {
      continue;
    }

    std::cout << k << ", ";
  }
  std::cout << "\b}\n";

  std::cout << " constexpr uint32_t POW_5_CACHE[] = { \n";
  for(int k = 0; k < 1024; k++)
  {
    if(k % 64 != 0)
    {
      continue;
    }
    // 308 decimal digits require 39 base-10^8 words
    constexpr unsigned NUM_WORDS = 39;
    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };
    SLOW_PRE_ARR[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 2 in each iteration)
    unsigned i;
    for(i = 0; i + 5 < k; i += 5)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = ((uint64_t)w << 5U) + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    const unsigned miss = k - i;
    uint32_t carry = 0;
    for(unsigned int &w : SLOW_PRE_ARR)
    {
      uint64_t pp = ((uint64_t)w << miss) + carry;
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = (uint32_t)(pp - carry * 100'000'000U);
    }

    // Find most significant non-zero chunk
    int top_word = NUM_WORDS - 1;
    while(top_word > 0 && SLOW_PRE_ARR[top_word] == 0)
    {
      --top_word;
    }

    std::cout << " // " << " k = " << k << "\n";
    int xx = 0, yy = 0;
    // Output formatted result
    for(int w = 0; w < NUM_WORDS; w++)
    {
      if(xx == 8)
      {
        if(yy == 0)
        {
          std::cout << "// __m256i single load cvtepied to __m512i \n";
        }
        else
        {
          std::cout << " // " << yy << " - anotherone \n";
        }
        xx = 0, yy++;
      }
      std::cout << SLOW_PRE_ARR[w] << ", ";
      xx++;
    }
    std::cout << " // " << yy << " - anotherone \n";
    std::cout << "\n";
  }
  std::cout << "\b};\n";

  return 0;
}

int test_original_vs_new_proposal()
{
  constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
  constexpr int SHIFT_10E8 = 57;
  constexpr unsigned NUM_WORDS = 40; // 40 words * 8 digits = 320 digits (fits 2^1023's 308 digits safely)

  for(int k = 0; k < 1024; k++)
  {
    std::array<uint32_t, NUM_WORDS> ORIG_ARR = { 0 };
    ORIG_ARR[0] = 1; // 2^0 = 1

    uint32_t bits_remaining = k;
    while(bits_remaining > 0)
    {
      uint32_t step = (bits_remaining > 6) ? 6 : bits_remaining;
      uint32_t multiplier = 1U << step;

      uint32_t carry = 0;
      for(uint32_t &w : ORIG_ARR)
      {
        uint64_t p = (uint64_t)w * multiplier + carry;
        carry = (uint32_t)((p * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(p - (uint64_t)carry * 100'000'000ULL);
      }
      bits_remaining -= step;
    }

    int top_word_orig = NUM_WORDS - 1;
    while(top_word_orig > 0 && ORIG_ARR[top_word_orig] == 0)
    {
      --top_word_orig;
    }

    std::string option_1 = std::to_string(ORIG_ARR[top_word_orig]);
    for(int w = top_word_orig - 1; w >= 0; --w)
    {
      char buf[9];
      std::snprintf(buf, sizeof(buf), "%08u", ORIG_ARR[w]);
      option_1 += buf;
    }

    std::array<uint32_t, NUM_WORDS> NEW_ARR = { 0 };
    NEW_ARR[0] = 1; // 2^0 = 1

    unsigned i = 0;
    for(; i + 5 < (unsigned)k; i += 5)
    {
      uint32_t carry = 0;
      for(uint32_t &w : NEW_ARR)
      {
        uint64_t pp = ((uint64_t)w << 5U) + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    const unsigned miss = k - i;
    uint32_t carry = 0;
    for(uint32_t &w : NEW_ARR)
    {
      uint64_t pp = ((uint64_t)w << miss) + carry;
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = (uint32_t)(pp - carry * 100'000'000U);
    }

    int top_word_new = NUM_WORDS - 1;
    while(top_word_new > 0 && NEW_ARR[top_word_new] == 0)
    {
      --top_word_new;
    }

    std::string option_2 = std::to_string(NEW_ARR[top_word_new]);
    for(int w = top_word_new - 1; w >= 0; --w)
    {
      char buf[9];
      std::snprintf(buf, sizeof(buf), "%08u", NEW_ARR[w]);
      option_2 += buf;
    }

    // Exact string equality assertion
    assert(option_1 == option_2);
  }

  std::cout << "All 1024 iterations matched perfectly!\n";
  return 0;
}

int new_proposal()
{
  for(int k = 1023; k < 1024; k++)
  {
    // 308 decimal digits require 39 base-10^8 words
    constexpr unsigned NUM_WORDS = 39;
    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };
    SLOW_PRE_ARR[0] = 1; // initialize 2^0 = 1

    // Loop k times (multiply by 2 in each iteration)
    unsigned i;
    for(i = 0; i + 5 < k; i += 5)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = ((uint64_t)w << 5U) + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    const unsigned miss = k - i;
    uint32_t carry = 0;
    for(unsigned int &w : SLOW_PRE_ARR)
    {
      uint64_t pp = ((uint64_t)w << miss) + carry;
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = (uint32_t)(pp - carry * 100'000'000U);
    }

    // Find most significant non-zero chunk
    int top_word = NUM_WORDS - 1;
    while(top_word > 0 && SLOW_PRE_ARR[top_word] == 0)
    {
      --top_word;
    }

    // Output formatted result
    std::cout << SLOW_PRE_ARR[top_word]; // Unpadded leading chunk
    for(int w = top_word - 1; w >= 0; --w)
    {
      std::cout << std::setfill('0') << std::setw(8) << SLOW_PRE_ARR[w];
    }
    std::cout << "\n";
  }

  return 0;
}

int original_algo()
{
  uint32_t k = 1024; // Test range [1, 1024]

  // 2^1024 has ~309 digits. 309 digits / 8 = ~39 chunks.
  // 40 words * 32 bits perfectly fits without any overflow.
  // ALL heap allocation is eliminated.
  constexpr int NUM_WORDS = 40;
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // 1. One-time Initialization Loop: Compute 2^k inline using Base-10^8
  uint32_t bits_remaining = k;
  while(bits_remaining > 0)
  {
    // Max step is 6 bits because (10^8 - 1) * 2^6 + carry
    // multiplied by our magic number safely fits inside a uint64_t.
    uint32_t step = (bits_remaining > 6) ? 6 : bits_remaining;
    uint32_t multiplier = 1U << step;

    uint32_t carry = 0; // Stored as u32
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Grows to u64 for safe intermediate math just like original snippet
      uint64_t p = (uint64_t)r[w] * multiplier + carry;

      // --- MAGIC NUMBER DIVISION (NO / OPERATOR) ---
      // Simulates: carry = p / 100,000,000
      carry = (uint32_t)((p * 1441151881ULL) >> 57);

      // --- MAGIC NUMBER MODULO (NO % OPERATOR) ---
      // Simulates: r[w] = p % 100,000,000
      r[w] = (uint32_t)(p - (uint64_t)carry * 100'000'000ULL);
    }
    bits_remaining -= step;
  }

  // Find the highest active chunk to drop leading zero chunks
  int start_word = NUM_WORDS - 1;
  while(start_word > 0 && r[start_word] == 0)
  {
    start_word--;
  }

  // 2. Main Processing Loop: Streams out 8-digit chunks exactly every iteration
  int chunk_count = 1;
  for(int w = start_word; w >= 0; --w)
  {
    uint32_t next8 = r[w]; // Extracted instantly in O(1)

    // Step E: Print immediately per iteration
    if(w == start_word)
    {
      // Very first chunk has no leading zeroes
      std::cout << "Chunk " << chunk_count++ << ": " << next8 << "\n";
    }
    else
    {
      // Middle/trailing chunks get padded to exactly 8 digits
      std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";
    }
  }

  return 0;
}

/*
#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  std::cout << ",{ ";
  for(uint32_t k = 1; k < 2048; k *= 2) // Supports any k in [1, 1100]
  {
    // 36 words * 32 bits is enough for k=1152
    constexpr int NUM_WORDS = 40;
    uint32_t r[NUM_WORDS] = { 0 };
    r[0] = 1; // Initialize R = 1

    // Magic number for exact division by 10^8 using only shifts and multiplies
    // M = ceil(2^60 / 10^8)
    constexpr uint64_t MAGIC_10_8 = 11529215047ULL;
    constexpr uint32_t SHIFT_VAL = 60;

    // 1. Compute exactly 2^k in Base-10^8 chunks
    for(uint32_t i = 0; i < k; ++i)
    {
      uint32_t carry = 0;
      for(int w = 0; w < NUM_WORDS; ++w)
      {
        // Max p = 99,999,999 * 2 + 1 = 199,999,999
        uint64_t p = ((uint64_t)r[w] << 1U) + carry;

        // division by 10^8 converted to multiply and shift
        carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);

        // modulo 10^8 converted to subtraction
        r[w] = (uint32_t)(p - ((uint64_t)carry * 100'000'000));
      }
    }

    std::cout << ",{ ";
    for(int n = 0; n < NUM_WORDS; n++)
    {
      std::cout << r[n] << ", ";
    }

    std::cout << "} \n " << std::endl;
  }
  std::cout << "} \n " << std::endl;
  return 0;
}
*/

/*

#include <cstdint>
#include <iomanip>
#include <iostream>

int main()
{
  uint32_t k = 512; // Supports any k in [1, 1024]

  // 36 words * 32 bits is enough for k=1152
  constexpr int NUM_WORDS = 40;
  uint32_t r[NUM_WORDS] = { 0 };
  r[0] = 1; // Initialize R = 1

  // Magic number for exact division by 10^8 using only shifts and multiplies
  // M = ceil(2^60 / 10^8)
  constexpr uint64_t MAGIC_10_8 = 11529215047ULL;
  constexpr uint32_t SHIFT_VAL = 60;

  // 1. Compute exactly 2^k in Base-10^8 chunks
  for(uint32_t i = 0; i < k; ++i)
  {
    uint32_t carry = 0;
    for(int w = 0; w < NUM_WORDS; ++w)
    {
      // Max p = 99,999,999 * 2 + 1 = 199,999,999
      uint64_t p = ((uint64_t)r[w] << 1U) + carry;

      // division by 10^8 converted to multiply and shift
      carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);

      // modulo 10^8 converted to subtraction
      r[w] = (uint32_t)(p - ((uint64_t)carry * 100'000'000));
    }
  }

  // 2. Find the most significant word (O(1) search)
  int top = NUM_WORDS - 1;
  while(top > 0 && r[top] == 0)
  {
    top--;
  }

  // 3. Align the most significant chunk to exactly 8 digits
  uint32_t top_val = r[top];
  uint32_t missing_digits = 0;
  uint32_t temp = top_val;

  // Count missing digits without division (temp *= 10 using shifts)
  while(temp < 10000000)
  {
    temp = (temp << 3) + (temp << 1);
    missing_digits++;
  }

  // 4. Cascade shift the entire array by 10^missing_digits to pull up lower digits
  for(uint32_t m = 0; m < missing_digits; ++m)
  {
    uint32_t carry = 0;
    for(int w = 0; w <= top; ++w)
    {
      // Max p = 99,999,999 * 10 + 9 = 999,999,999
      uint64_t p = (uint64_t)r[w] * 10ULL + carry;
      carry = (uint32_t)((p * MAGIC_10_8) >> SHIFT_VAL);
      r[w] = (uint32_t)(p - carry * 100000000ULL);
    }
    // If the shift caused an overflow into a new chunk, advance the top pointer
    if(carry > 0)
    {
      top++;
      r[top] = carry;
    }
  }

  for(int n = NUM_WORDS - 1; n >= 0; n--)
  {
    std::cout << std::setfill('0') << std::setw(8) << r[n];
  }

  std::cout << std::endl;

  return 0;
}
*/

//
