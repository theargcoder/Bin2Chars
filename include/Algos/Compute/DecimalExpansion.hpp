#pragma once

#include <array>
#include <cstdint>
#include <immintrin.h>

namespace Algos::Compute::DecimalExpansion
{
  namespace Exponent
  {
    namespace Positive
    {
      static constexpr uint16_t POW_2_E[] = { 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024 };
      static constexpr uint32_t POW_2_CACHE[] = {
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
        38787518, 80353382, 13999990, 74531401, 9, 0, 0, 0,                            // 4 - anotherone

        //  k = 1024
        24137216, 63296242, 30483535, 38479716, 99472459, 58629823, 50510684, 72371633, // __m256i single load cvtepied to __m512i
        11054082, 81473913, 23424628, 83815068, 85005768, 30829520, 9411945, 19601246,  // 1 - anotherone
        52763022, 42486548, 77767893, 94741243, 84743063, 16622492, 97688144, 35765878, // 2 - anotherone
        79871393, 11201138, 40753602, 8477322, 9631327, 67580550, 81157732, 72734300,   // 3 - anotherone
        89423065, 61797697, 89024733, 93051907, 31590772, 93134862, 17976, 0            // 4 - anotherone
      };
    } // namespace Positive

    namespace Negative
    {
      constexpr uint16_t POW_5_E[] = { 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024 };
      constexpr uint32_t POW_5_CACHE[] = {
        //  k = 0
        1, 0, 0, 0, 0, 0, 0, 0, // __m256i single load cvtepied to __m512i
        0, 0, 0, 0, 0, 0, 0, 0, // 1 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 2 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 3 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0, // 11 - anotherone

        //  k = 64
        12890625, 49708557, 72640043, 52217003, 10862427, 54210, 0, 0, // __m256i single load cvtepied to __m512i
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 1 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 2 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 3 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                        // 11 - anotherone

        //  k = 128
        12890625, 36817932, 60431486, 87926569, 18803771, 38919302, 19454666, 43055614, // __m256i single load cvtepied to __m512i
        99218413, 5571876, 38735877, 29, 0, 0, 0, 0,                                    // 1 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 2 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 3 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 192
        12890625, 23927307, 61113554, 98344692, 82685940, 98747201, 16094308, 1513058, // __m256i single load cvtepied to __m512i
        33861629, 7388585, 92618786, 11045551, 71180559, 80397767, 22770288, 91113245, // 1 - anotherone
        1593091, 0, 0, 0, 0, 0, 0, 0,                                                  // 2 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 3 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                        // 11 - anotherone

        //  k = 256
        12890625, 11036682, 74686247, 70580747, 65300377, 31946693, 50989468, 66359699, // __m256i single load cvtepied to __m512i
        93946149, 65605472, 47228822, 71507503, 24270579, 59180316, 3470168, 13850237,  // 1 - anotherone
        36443628, 71116000, 28003995, 38635186, 94444625, 61685550, 863, 0,             // 2 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 3 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 320
        12890625, 98146057, 1149564, 91744110, 42329149, 90635119, 24452290, 74541517,  // __m256i single load cvtepied to __m512i
        28927090, 92338075, 20461226, 19914931, 91753374, 42001963, 53790631, 46334430, // 1 - anotherone
        9753926, 49486635, 66739996, 86038327, 37155453, 41833163, 2333879, 76569915,   // 2 - anotherone
        94138586, 32715584, 54692198, 46816763, 0, 0, 0, 0,                             // 3 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 384
        12890625, 85255432, 40503507, 48944154, 2344950, 13747753, 13827972, 78071105,  // __m256i single load cvtepied to __m512i
        52827180, 38068409, 82066824, 66882658, 97356950, 43871649, 76179778, 7063576,  // 1 - anotherone
        62670278, 71342443, 58466020, 5775089, 29664591, 57842862, 40742905, 99473932,  // 2 - anotherone
        66978892, 32123947, 81606060, 97010955, 82361884, 30543546, 40245558, 64922327, // 3 - anotherone
        41837315, 25379, 0, 0, 0, 0, 0, 0,                                              // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 448
        12890625, 72364807, 92748075, 29290255, 46811097, 14147175, 24176824, 37844232, // __m256i single load cvtepied to __m512i
        24728885, 98848681, 81258974, 48523067, 36399244, 28159503, 26488589, 45224459, // 1 - anotherone
        99663056, 31543307, 40942492, 890775, 77556159, 38689000, 30766881, 80972948,   // 2 - anotherone
        81289743, 20078646, 98486656, 51936415, 20462563, 22803578, 87165186, 45187091, // 3 - anotherone
        48268780, 24664295, 51613602, 52617006, 63667897, 82973977, 75821026, 13,       // 4 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 512
        12890625, 59474182, 57883268, 19891789, 90081533, 65732649, 51846968, 29274335, // __m256i single load cvtepied to __m512i
        64600361, 36516628, 72060583, 35021533, 63699265, 27422975, 82300322, 20031686, // 1 - anotherone
        5885071, 41421726, 63914765, 65543650, 26294144, 74266435, 74952040, 31878137,  // 2 - anotherone
        81260792, 20846914, 16482959, 2660486, 80952253, 45341272, 55836101, 15116032,  // 3 - anotherone
        42780336, 74890400, 83049361, 38507011, 33206278, 42715183, 60040689, 76471534, // 4 - anotherone
        29338373, 96531546, 6743290, 7312002, 745834, 0, 0, 0,                          // 5 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 576
        12890625, 46583557, 35909086, 7858130, 59400826, 90549496, 60937654, 46854319,  // __m256i single load cvtepied to __m512i
        30137308, 47438283, 40813271, 64976600, 99836803, 69530310, 37659264, 40881129, // 1 - anotherone
        79372006, 10121537, 22022885, 60098026, 28182903, 58853540, 26431305, 30076959, // 2 - anotherone
        11917655, 58476191, 16558628, 65837500, 82196683, 26392072, 96450387, 62626466, // 3 - anotherone
        89528634, 65810762, 18636822, 5450082, 65758491, 88431705, 17610809, 11360491,  // 4 - anotherone
        68817242, 83185410, 85806158, 32411358, 92188827, 32843323, 34379260, 63059953, // 5 - anotherone
        52194906, 31746119, 404, 0, 0, 0, 0, 0,                                         // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 640
        12890625, 33692932, 26825529, 80298653, 94904168, 45898467, 72653202, 94755038, // __m256i single load cvtepied to __m512i
        93193412, 52111858, 33905426, 43312635, 4316200, 9652344, 46355256, 36557909,   // 1 - anotherone
        57829271, 66908547, 57223525, 88200147, 85245514, 75901101, 25534275, 5366715,  // 2 - anotherone
        80307590, 56656847, 94522357, 53852460, 27196536, 63455020, 97146204, 38187954, // 3 - anotherone
        9246733, 63878272, 38062796, 57688470, 11380580, 40626592, 30234391, 26766131,  // 4 - anotherone
        69340090, 31925646, 92594249, 45371758, 9166570, 38274147, 13155347, 39061475,  // 5 - anotherone
        72604608, 40366232, 80872212, 20832630, 31071411, 3975269, 49008403, 21918093,  // 6 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 704
        12890625, 20802307, 30632597, 24322733, 49617379, 11445123, 67547569, 50569835, // __m256i single load cvtepied to __m512i
        67262698, 1893786, 78092516, 84994366, 74064624, 72924940, 16030810, 81633289,  // 1 - anotherone
        41636990, 53924495, 27432587, 65553471, 66057806, 38905810, 40844438, 76217811, // 2 - anotherone
        63932947, 14708443, 440751, 75439013, 52241383, 12526408, 61240168, 71398916,   // 3 - anotherone
        9470344, 24352410, 89888501, 40012985, 60073909, 70502138, 55961549, 20570223,  // 4 - anotherone
        56448341, 36740442, 7829986, 53903649, 88968971, 14027367, 79217364, 43956136,  // 5 - anotherone
        1631643, 78886645, 25093565, 20127223, 89057984, 28597865, 95276091, 39834493,  // 6 - anotherone
        29054450, 46939443, 69202536, 47488969, 82228934, 11881, 0, 0,                  // 7 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 768
        12890625, 7911682, 47330290, 27039745, 89456900, 76329204, 659543, 89314513,    // __m256i single load cvtepied to __m512i
        50367729, 10144597, 93672434, 70227291, 59607713, 21605513, 98823779, 55991991, // 1 - anotherone
        97529227, 95394377, 33456482, 71686865, 68536250, 77223259, 94059799, 83506650, // 2 - anotherone
        26685244, 91147685, 42664879, 23577135, 38846903, 89874517, 36838071, 58163972, // 3 - anotherone
        2141121, 75557931, 29152176, 67930773, 58553576, 99696070, 20696027, 2342593,   // 4 - anotherone
        47840348, 27056717, 28634380, 7259100, 4927887, 80605088, 29537590, 13745453,   // 5 - anotherone
        59045222, 74645734, 71904625, 49474722, 49146671, 85621808, 61260668, 89725225, // 6 - anotherone
        3696462, 26932756, 32293236, 30757099, 65633584, 13314439, 25052048, 50336885,  // 7 - anotherone
        8227036, 95971333, 44114876, 6, 0, 0, 0, 0,                                     // 8 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 832
        12890625, 95021057, 76918607, 75559064, 93229799, 26274010, 29538564, 54792111, // __m256i single load cvtepied to __m512i
        60598235, 13238409, 35131333, 48242770, 76832770, 70923846, 99238382, 50433919, // 1 - anotherone
        24468835, 50788035, 70468258, 37184642, 60475589, 84813955, 6596266, 71600613,  // 2 - anotherone
        50284037, 77518631, 32652230, 94682357, 28203237, 82573046, 73310462, 99146738, // 3 - anotherone
        69153691, 58415613, 47288467, 70587186, 45305868, 38022974, 51159120, 26742109, // 4 - anotherone
        74165051, 70971848, 92549028, 60177187, 36556661, 39740493, 20587097, 72783066, // 5 - anotherone
        85408841, 74337366, 95639295, 12736340, 95934324, 74825701, 26051124, 43191684, // 6 - anotherone
        15649069, 56455311, 37868484, 34231007, 84417061, 29504413, 20783371, 74337233, // 7 - anotherone
        21855713, 85509018, 10019730, 23541746, 12938018, 17783003, 72892747, 37446497, // 8 - anotherone
        349175, 0, 0, 0, 0, 0, 0, 0,                                                    // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 896
        12890625, 82130432, 19397550, 56990066, 52633770, 30695772, 55161160, 65431869, // __m256i single load cvtepied to __m512i
        54435342, 52135210, 67582027, 22178361, 45662149, 91858009, 88417422, 79935803, // 1 - anotherone
        77224690, 78333901, 23956413, 37558796, 54784028, 69676666, 94248976, 24391821, // 2 - anotherone
        65565657, 44986285, 95772611, 27224092, 39702911, 49202196, 74595943, 14175039, // 3 - anotherone
        45896806, 17481105, 14683103, 41547070, 15883199, 37433330, 45316856, 6328403,  // 4 - anotherone
        37165440, 8686134, 73496245, 29234248, 895025, 36001475, 57385367, 66451464,    // 5 - anotherone
        20381473, 27116116, 31065816, 18604120, 69119138, 65080674, 31586602, 3085059,  // 6 - anotherone
        39906385, 91257172, 11614740, 87190497, 57480764, 92504006, 55350450, 98520204, // 7 - anotherone
        84173687, 48798656, 2505297, 49206327, 26707219, 73252581, 60322630, 54838450,  // 8 - anotherone
        49424763, 50643491, 60288424, 55640255, 66839537, 28834978, 189, 0,             // 9 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 960
        12890625, 69239807, 74767118, 58442124, 72257130, 29813027, 75738021, 41711742, // __m256i single load cvtepied to __m512i
        30887242, 37579931, 13268257, 90118688, 25870432, 38707920, 92621288, 75004620, // 1 - anotherone
        3249224, 33315057, 63932247, 64952842, 27029643, 21294253, 95447096, 47773580,  // 2 - anotherone
        69229590, 88712012, 66558197, 56291399, 90348349, 71668038, 50909283, 85414400, // 3 - anotherone
        61078027, 42478563, 10377116, 68993929, 89380307, 47371512, 89712946, 85362783, // 4 - anotherone
        24296365, 65366613, 71319642, 42647562, 5355883, 3947957, 25684223, 70221991,   // 5 - anotherone
        65133714, 87986168, 47983985, 99509759, 20234596, 25749706, 79412382, 11776446, // 6 - anotherone
        73970722, 96090587, 90448513, 7921097, 39986773, 56120138, 98063699, 88753174,  // 7 - anotherone
        83650732, 60992752, 71803363, 59593387, 12255325, 38513683, 7561870, 91329359,  // 8 - anotherone
        83375844, 48343160, 40068095, 38474475, 5222620, 8028993, 90530683, 82968521,   // 9 - anotherone
        73222912, 6233400, 324594, 10261342, 0, 0, 0, 0,                                // 10 - anotherone
        0, 0, 0, 0, 0, 0, 0, 0,                                                         // 11 - anotherone

        //  k = 1024
        12890625, 56349182, 43027311, 67024615, 69578822, 21755996, 53411689, 42273851, // __m256i single load cvtepied to __m512i
        37352728, 21466491, 93494493, 17315231, 76695971, 47245793, 8335217, 42684328,  // 1 - anotherone
        94724182, 57903306, 18822547, 71271179, 76350385, 64179153, 61855404, 75060403, // 2 - anotherone
        46331102, 65700422, 34778692, 70285190, 10511211, 7071594, 8771803, 45371416,   // 3 - anotherone
        18314079, 34294051, 96628257, 84158693, 86657834, 60377765, 29007481, 39782222, // 4 - anotherone
        98924902, 80975371, 33802858, 12998108, 52714415, 24802899, 84483951, 46612783, // 5 - anotherone
        14561770, 28849964, 29012627, 36289511, 38437487, 99452219, 57621869, 77192529, // 6 - anotherone
        21251589, 14790991, 22794992, 97371801, 65416453, 12701662, 35520938, 73562593, // 7 - anotherone
        80391261, 59090179, 53686064, 82619902, 83830005, 28269449, 44429705, 40030963, // 8 - anotherone
        36502721, 8540388, 27512881, 93275895, 41016834, 31277551, 57078975, 79548963,  // 9 - anotherone
        11087034, 85422180, 57638331, 51155829, 5480399, 33101016, 25581793, 80034577,  // 10 - anotherone
        68464626, 5562, 0, 0, 0, 0, 0, 0                                                // 11 - anotherone
      };
    } // namespace Negative
  } // namespace Exponent

  constexpr auto MAX_ARRAY_SIZE = 96;

#if defined(__AVX512BW__) && defined(__AVX512VL__)
  void PositiveExponent(std::array<unsigned, MAX_ARRAY_SIZE> &SIMD_ARRAY, const int &exponent)
  {
    const auto &POW_2_E = Exponent::Positive::POW_2_E;
    const auto &POW_2_CACHE = Exponent::Positive::POW_2_CACHE;

    const unsigned EXP_DIV_64 = static_cast<unsigned>(exponent) >> 6U;
    const unsigned BASE_IDX = EXP_DIV_64 * 40;
    const unsigned E_0 = POW_2_E[EXP_DIV_64];

    const __m512i ZERO = _mm512_setzero_si512();

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_2_CACHE[BASE_IDX])));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_2_CACHE[BASE_IDX + 8])));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_2_CACHE[BASE_IDX + 16])));
    __m512i rrprime_4 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_2_CACHE[BASE_IDX + 24])));
    __m512i rrprime_5 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_2_CACHE[BASE_IDX + 32])));

    const __m512i R_10E8 = _mm512_set1_epi64(100'000'000U);
    const __m512i R_MAGIC_10E8 = _mm512_set1_epi64(1'441'151'881U);

    int e = E_0;
    for(; e + 5 < exponent; e += 5) // each iteration has ~25 cycle latency --- since 8 iterations is worst case then 200 cycle latency just in this loop ...
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

    const auto missing = static_cast<unsigned>(exponent - e);

    if(missing)
    {
      const __m512i u64_prod_1 = _mm512_slli_epi64(rrprime_1, missing);
      const __m512i u64_prod_2 = _mm512_slli_epi64(rrprime_2, missing);
      const __m512i u64_prod_3 = _mm512_slli_epi64(rrprime_3, missing);
      const __m512i u64_prod_4 = _mm512_slli_epi64(rrprime_4, missing);
      const __m512i u64_prod_5 = _mm512_slli_epi64(rrprime_5, missing);
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
    const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
    const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
    const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
    const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
    const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), u32_pack_1);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), u32_pack_2);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), u32_pack_3);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), u32_pack_4);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[32]), u32_pack_5);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[40]), _mm512_castsi512_si256(ZERO));
    _mm512_storeu_si512(&SIMD_ARRAY[48], ZERO);
    _mm512_storeu_si512(&SIMD_ARRAY[64], ZERO);
    _mm512_storeu_si512(&SIMD_ARRAY[80], ZERO);
  }

  void NegativeExponent(std::array<unsigned, MAX_ARRAY_SIZE> &SIMD_ARRAY, const int &exponent)
  {
    const auto &POW_5_E = Exponent::Negative::POW_5_E;
    const auto &POW_5_CACHE = Exponent::Negative::POW_5_CACHE;

    const unsigned K_DIV_64 = static_cast<unsigned>(exponent) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 96;
    const unsigned E_0 = POW_5_E[K_DIV_64];

    const __m512i ZERO = _mm512_setzero_si512();

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX])));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 8])));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 16])));
    __m512i rrprime_4 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 24])));
    __m512i rrprime_5 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 32])));
    __m512i rrprime_6 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 40])));
    __m512i rrprime_7 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 48])));
    __m512i rrprime_8 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 56])));
    __m512i rrprime_9 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 64])));
    __m512i rrprime_10 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 72])));
    __m512i rrprime_11 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 80])));
    __m512i rrprime_12 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[BASE_IDX + 88])));

    const __m512i R_25 = _mm512_set1_epi64(25U);
    const __m512i R_10E8 = _mm512_set1_epi64(100'000'000U);
    const __m512i R_MAGIC_10E8 = _mm512_set1_epi64(1'441'151'881U);

    for(int e = E_0 + 2; e <= exponent; e += 2) // each iteration has ~74 cycle latency --- since 32 iterations is worst case then 2368 cycle latency just in this loop ...
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, R_25);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, R_25);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, R_25);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, R_25);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, R_25);
      const __m512i u64_prod_6 = _mm512_mul_epu32(rrprime_6, R_25);
      const __m512i u64_prod_7 = _mm512_mul_epu32(rrprime_7, R_25);
      const __m512i u64_prod_8 = _mm512_mul_epu32(rrprime_8, R_25);
      const __m512i u64_prod_9 = _mm512_mul_epu32(rrprime_9, R_25);
      const __m512i u64_prod_10 = _mm512_mul_epu32(rrprime_10, R_25);
      const __m512i u64_prod_11 = _mm512_mul_epu32(rrprime_11, R_25);
      const __m512i u64_prod_12 = _mm512_mul_epu32(rrprime_12, R_25);
      const __m512i u64_magic_prod_1 = _mm512_mul_epu32(u64_prod_1, R_MAGIC_10E8);
      const __m512i u64_magic_prod_2 = _mm512_mul_epu32(u64_prod_2, R_MAGIC_10E8);
      const __m512i u64_magic_prod_3 = _mm512_mul_epu32(u64_prod_3, R_MAGIC_10E8);
      const __m512i u64_magic_prod_4 = _mm512_mul_epu32(u64_prod_4, R_MAGIC_10E8);
      const __m512i u64_magic_prod_5 = _mm512_mul_epu32(u64_prod_5, R_MAGIC_10E8);
      const __m512i u64_magic_prod_6 = _mm512_mul_epu32(u64_prod_6, R_MAGIC_10E8);
      const __m512i u64_magic_prod_7 = _mm512_mul_epu32(u64_prod_7, R_MAGIC_10E8);
      const __m512i u64_magic_prod_8 = _mm512_mul_epu32(u64_prod_8, R_MAGIC_10E8);
      const __m512i u64_magic_prod_9 = _mm512_mul_epu32(u64_prod_9, R_MAGIC_10E8);
      const __m512i u64_magic_prod_10 = _mm512_mul_epu32(u64_prod_10, R_MAGIC_10E8);
      const __m512i u64_magic_prod_11 = _mm512_mul_epu32(u64_prod_11, R_MAGIC_10E8);
      const __m512i u64_magic_prod_12 = _mm512_mul_epu32(u64_prod_12, R_MAGIC_10E8);
      const __m512i u64_magic_res_1 = _mm512_srli_epi64(u64_magic_prod_1, 57U);
      const __m512i u64_magic_res_2 = _mm512_srli_epi64(u64_magic_prod_2, 57U);
      const __m512i u64_magic_res_3 = _mm512_srli_epi64(u64_magic_prod_3, 57U);
      const __m512i u64_magic_res_4 = _mm512_srli_epi64(u64_magic_prod_4, 57U);
      const __m512i u64_magic_res_5 = _mm512_srli_epi64(u64_magic_prod_5, 57U);
      const __m512i u64_magic_res_6 = _mm512_srli_epi64(u64_magic_prod_6, 57U);
      const __m512i u64_magic_res_7 = _mm512_srli_epi64(u64_magic_prod_7, 57U);
      const __m512i u64_magic_res_8 = _mm512_srli_epi64(u64_magic_prod_8, 57U);
      const __m512i u64_magic_res_9 = _mm512_srli_epi64(u64_magic_prod_9, 57U);
      const __m512i u64_magic_res_10 = _mm512_srli_epi64(u64_magic_prod_10, 57U);
      const __m512i u64_magic_res_11 = _mm512_srli_epi64(u64_magic_prod_11, 57U);
      const __m512i u64_magic_res_12 = _mm512_srli_epi64(u64_magic_prod_12, 57U);
      const __m512i u64_magic_res_x10E8_1 = _mm512_mul_epu32(u64_magic_res_1, R_10E8);
      const __m512i u64_magic_res_x10E8_2 = _mm512_mul_epu32(u64_magic_res_2, R_10E8);
      const __m512i u64_magic_res_x10E8_3 = _mm512_mul_epu32(u64_magic_res_3, R_10E8);
      const __m512i u64_magic_res_x10E8_4 = _mm512_mul_epu32(u64_magic_res_4, R_10E8);
      const __m512i u64_magic_res_x10E8_5 = _mm512_mul_epu32(u64_magic_res_5, R_10E8);
      const __m512i u64_magic_res_x10E8_6 = _mm512_mul_epu32(u64_magic_res_6, R_10E8);
      const __m512i u64_magic_res_x10E8_7 = _mm512_mul_epu32(u64_magic_res_7, R_10E8);
      const __m512i u64_magic_res_x10E8_8 = _mm512_mul_epu32(u64_magic_res_8, R_10E8);
      const __m512i u64_magic_res_x10E8_9 = _mm512_mul_epu32(u64_magic_res_9, R_10E8);
      const __m512i u64_magic_res_x10E8_10 = _mm512_mul_epu32(u64_magic_res_10, R_10E8);
      const __m512i u64_magic_res_x10E8_11 = _mm512_mul_epu32(u64_magic_res_11, R_10E8);
      const __m512i u64_magic_res_x10E8_12 = _mm512_mul_epu32(u64_magic_res_12, R_10E8);
      const __m512i u64_magic_carry_slide_1 = _mm512_alignr_epi64(u64_magic_res_1, ZERO, 7);
      const __m512i u64_magic_carry_slide_2 = _mm512_alignr_epi64(u64_magic_res_2, u64_magic_res_1, 7);
      const __m512i u64_magic_carry_slide_3 = _mm512_alignr_epi64(u64_magic_res_3, u64_magic_res_2, 7);
      const __m512i u64_magic_carry_slide_4 = _mm512_alignr_epi64(u64_magic_res_4, u64_magic_res_3, 7);
      const __m512i u64_magic_carry_slide_5 = _mm512_alignr_epi64(u64_magic_res_5, u64_magic_res_4, 7);
      const __m512i u64_magic_carry_slide_6 = _mm512_alignr_epi64(u64_magic_res_6, u64_magic_res_5, 7);
      const __m512i u64_magic_carry_slide_7 = _mm512_alignr_epi64(u64_magic_res_7, u64_magic_res_6, 7);
      const __m512i u64_magic_carry_slide_8 = _mm512_alignr_epi64(u64_magic_res_8, u64_magic_res_7, 7);
      const __m512i u64_magic_carry_slide_9 = _mm512_alignr_epi64(u64_magic_res_9, u64_magic_res_8, 7);
      const __m512i u64_magic_carry_slide_10 = _mm512_alignr_epi64(u64_magic_res_10, u64_magic_res_9, 7);
      const __m512i u64_magic_carry_slide_11 = _mm512_alignr_epi64(u64_magic_res_11, u64_magic_res_10, 7);
      const __m512i u64_magic_carry_slide_12 = _mm512_alignr_epi64(u64_magic_res_12, u64_magic_res_11, 7);
      rrprime_1 = _mm512_add_epi64(u64_prod_1, u64_magic_carry_slide_1);
      rrprime_2 = _mm512_add_epi64(u64_prod_2, u64_magic_carry_slide_2);
      rrprime_3 = _mm512_add_epi64(u64_prod_3, u64_magic_carry_slide_3);
      rrprime_4 = _mm512_add_epi64(u64_prod_4, u64_magic_carry_slide_4);
      rrprime_5 = _mm512_add_epi64(u64_prod_5, u64_magic_carry_slide_5);
      rrprime_6 = _mm512_add_epi64(u64_prod_6, u64_magic_carry_slide_6);
      rrprime_7 = _mm512_add_epi64(u64_prod_7, u64_magic_carry_slide_7);
      rrprime_8 = _mm512_add_epi64(u64_prod_8, u64_magic_carry_slide_8);
      rrprime_9 = _mm512_add_epi64(u64_prod_9, u64_magic_carry_slide_9);
      rrprime_10 = _mm512_add_epi64(u64_prod_10, u64_magic_carry_slide_10);
      rrprime_11 = _mm512_add_epi64(u64_prod_11, u64_magic_carry_slide_11);
      rrprime_12 = _mm512_add_epi64(u64_prod_12, u64_magic_carry_slide_12);

      rrprime_1 = _mm512_sub_epi32(rrprime_1, u64_magic_res_x10E8_1);
      rrprime_2 = _mm512_sub_epi32(rrprime_2, u64_magic_res_x10E8_2);
      rrprime_3 = _mm512_sub_epi32(rrprime_3, u64_magic_res_x10E8_3);
      rrprime_4 = _mm512_sub_epi32(rrprime_4, u64_magic_res_x10E8_4);
      rrprime_5 = _mm512_sub_epi32(rrprime_5, u64_magic_res_x10E8_5);
      rrprime_6 = _mm512_sub_epi32(rrprime_6, u64_magic_res_x10E8_6);
      rrprime_7 = _mm512_sub_epi32(rrprime_7, u64_magic_res_x10E8_7);
      rrprime_8 = _mm512_sub_epi32(rrprime_8, u64_magic_res_x10E8_8);
      rrprime_9 = _mm512_sub_epi32(rrprime_9, u64_magic_res_x10E8_9);
      rrprime_10 = _mm512_sub_epi32(rrprime_10, u64_magic_res_x10E8_10);
      rrprime_11 = _mm512_sub_epi32(rrprime_11, u64_magic_res_x10E8_11);
      rrprime_12 = _mm512_sub_epi32(rrprime_12, u64_magic_res_x10E8_12);
    }

    if((exponent & 0b1) != 0) // if its odd...
    {
      const __m512i R_5 = _mm512_set1_epi64(5U);
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, R_5);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, R_5);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, R_5);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, R_5);
      const __m512i u64_prod_5 = _mm512_mul_epu32(rrprime_5, R_5);
      const __m512i u64_prod_6 = _mm512_mul_epu32(rrprime_6, R_5);
      const __m512i u64_prod_7 = _mm512_mul_epu32(rrprime_7, R_5);
      const __m512i u64_prod_8 = _mm512_mul_epu32(rrprime_8, R_5);
      const __m512i u64_prod_9 = _mm512_mul_epu32(rrprime_9, R_5);
      const __m512i u64_prod_10 = _mm512_mul_epu32(rrprime_10, R_5);
      const __m512i u64_prod_11 = _mm512_mul_epu32(rrprime_11, R_5);
      const __m512i u64_prod_12 = _mm512_mul_epu32(rrprime_12, R_5);
      const __m512i u64_magic_prod_1 = _mm512_mul_epu32(u64_prod_1, R_MAGIC_10E8);
      const __m512i u64_magic_prod_2 = _mm512_mul_epu32(u64_prod_2, R_MAGIC_10E8);
      const __m512i u64_magic_prod_3 = _mm512_mul_epu32(u64_prod_3, R_MAGIC_10E8);
      const __m512i u64_magic_prod_4 = _mm512_mul_epu32(u64_prod_4, R_MAGIC_10E8);
      const __m512i u64_magic_prod_5 = _mm512_mul_epu32(u64_prod_5, R_MAGIC_10E8);
      const __m512i u64_magic_prod_6 = _mm512_mul_epu32(u64_prod_6, R_MAGIC_10E8);
      const __m512i u64_magic_prod_7 = _mm512_mul_epu32(u64_prod_7, R_MAGIC_10E8);
      const __m512i u64_magic_prod_8 = _mm512_mul_epu32(u64_prod_8, R_MAGIC_10E8);
      const __m512i u64_magic_prod_9 = _mm512_mul_epu32(u64_prod_9, R_MAGIC_10E8);
      const __m512i u64_magic_prod_10 = _mm512_mul_epu32(u64_prod_10, R_MAGIC_10E8);
      const __m512i u64_magic_prod_11 = _mm512_mul_epu32(u64_prod_11, R_MAGIC_10E8);
      const __m512i u64_magic_prod_12 = _mm512_mul_epu32(u64_prod_12, R_MAGIC_10E8);
      const __m512i u64_magic_res_1 = _mm512_srli_epi64(u64_magic_prod_1, 57U);
      const __m512i u64_magic_res_2 = _mm512_srli_epi64(u64_magic_prod_2, 57U);
      const __m512i u64_magic_res_3 = _mm512_srli_epi64(u64_magic_prod_3, 57U);
      const __m512i u64_magic_res_4 = _mm512_srli_epi64(u64_magic_prod_4, 57U);
      const __m512i u64_magic_res_5 = _mm512_srli_epi64(u64_magic_prod_5, 57U);
      const __m512i u64_magic_res_6 = _mm512_srli_epi64(u64_magic_prod_6, 57U);
      const __m512i u64_magic_res_7 = _mm512_srli_epi64(u64_magic_prod_7, 57U);
      const __m512i u64_magic_res_8 = _mm512_srli_epi64(u64_magic_prod_8, 57U);
      const __m512i u64_magic_res_9 = _mm512_srli_epi64(u64_magic_prod_9, 57U);
      const __m512i u64_magic_res_10 = _mm512_srli_epi64(u64_magic_prod_10, 57U);
      const __m512i u64_magic_res_11 = _mm512_srli_epi64(u64_magic_prod_11, 57U);
      const __m512i u64_magic_res_12 = _mm512_srli_epi64(u64_magic_prod_12, 57U);
      const __m512i u64_magic_res_x10E8_1 = _mm512_mul_epu32(u64_magic_res_1, R_10E8);
      const __m512i u64_magic_res_x10E8_2 = _mm512_mul_epu32(u64_magic_res_2, R_10E8);
      const __m512i u64_magic_res_x10E8_3 = _mm512_mul_epu32(u64_magic_res_3, R_10E8);
      const __m512i u64_magic_res_x10E8_4 = _mm512_mul_epu32(u64_magic_res_4, R_10E8);
      const __m512i u64_magic_res_x10E8_5 = _mm512_mul_epu32(u64_magic_res_5, R_10E8);
      const __m512i u64_magic_res_x10E8_6 = _mm512_mul_epu32(u64_magic_res_6, R_10E8);
      const __m512i u64_magic_res_x10E8_7 = _mm512_mul_epu32(u64_magic_res_7, R_10E8);
      const __m512i u64_magic_res_x10E8_8 = _mm512_mul_epu32(u64_magic_res_8, R_10E8);
      const __m512i u64_magic_res_x10E8_9 = _mm512_mul_epu32(u64_magic_res_9, R_10E8);
      const __m512i u64_magic_res_x10E8_10 = _mm512_mul_epu32(u64_magic_res_10, R_10E8);
      const __m512i u64_magic_res_x10E8_11 = _mm512_mul_epu32(u64_magic_res_11, R_10E8);
      const __m512i u64_magic_res_x10E8_12 = _mm512_mul_epu32(u64_magic_res_12, R_10E8);
      const __m512i u64_magic_carry_slide_1 = _mm512_alignr_epi64(u64_magic_res_1, ZERO, 7);
      const __m512i u64_magic_carry_slide_2 = _mm512_alignr_epi64(u64_magic_res_2, u64_magic_res_1, 7);
      const __m512i u64_magic_carry_slide_3 = _mm512_alignr_epi64(u64_magic_res_3, u64_magic_res_2, 7);
      const __m512i u64_magic_carry_slide_4 = _mm512_alignr_epi64(u64_magic_res_4, u64_magic_res_3, 7);
      const __m512i u64_magic_carry_slide_5 = _mm512_alignr_epi64(u64_magic_res_5, u64_magic_res_4, 7);
      const __m512i u64_magic_carry_slide_6 = _mm512_alignr_epi64(u64_magic_res_6, u64_magic_res_5, 7);
      const __m512i u64_magic_carry_slide_7 = _mm512_alignr_epi64(u64_magic_res_7, u64_magic_res_6, 7);
      const __m512i u64_magic_carry_slide_8 = _mm512_alignr_epi64(u64_magic_res_8, u64_magic_res_7, 7);
      const __m512i u64_magic_carry_slide_9 = _mm512_alignr_epi64(u64_magic_res_9, u64_magic_res_8, 7);
      const __m512i u64_magic_carry_slide_10 = _mm512_alignr_epi64(u64_magic_res_10, u64_magic_res_9, 7);
      const __m512i u64_magic_carry_slide_11 = _mm512_alignr_epi64(u64_magic_res_11, u64_magic_res_10, 7);
      const __m512i u64_magic_carry_slide_12 = _mm512_alignr_epi64(u64_magic_res_12, u64_magic_res_11, 7);
      rrprime_1 = _mm512_add_epi64(u64_prod_1, u64_magic_carry_slide_1);
      rrprime_2 = _mm512_add_epi64(u64_prod_2, u64_magic_carry_slide_2);
      rrprime_3 = _mm512_add_epi64(u64_prod_3, u64_magic_carry_slide_3);
      rrprime_4 = _mm512_add_epi64(u64_prod_4, u64_magic_carry_slide_4);
      rrprime_5 = _mm512_add_epi64(u64_prod_5, u64_magic_carry_slide_5);
      rrprime_6 = _mm512_add_epi64(u64_prod_6, u64_magic_carry_slide_6);
      rrprime_7 = _mm512_add_epi64(u64_prod_7, u64_magic_carry_slide_7);
      rrprime_8 = _mm512_add_epi64(u64_prod_8, u64_magic_carry_slide_8);
      rrprime_9 = _mm512_add_epi64(u64_prod_9, u64_magic_carry_slide_9);
      rrprime_10 = _mm512_add_epi64(u64_prod_10, u64_magic_carry_slide_10);
      rrprime_11 = _mm512_add_epi64(u64_prod_11, u64_magic_carry_slide_11);
      rrprime_12 = _mm512_add_epi64(u64_prod_12, u64_magic_carry_slide_12);

      rrprime_1 = _mm512_sub_epi32(rrprime_1, u64_magic_res_x10E8_1);
      rrprime_2 = _mm512_sub_epi32(rrprime_2, u64_magic_res_x10E8_2);
      rrprime_3 = _mm512_sub_epi32(rrprime_3, u64_magic_res_x10E8_3);
      rrprime_4 = _mm512_sub_epi32(rrprime_4, u64_magic_res_x10E8_4);
      rrprime_5 = _mm512_sub_epi32(rrprime_5, u64_magic_res_x10E8_5);
      rrprime_6 = _mm512_sub_epi32(rrprime_6, u64_magic_res_x10E8_6);
      rrprime_7 = _mm512_sub_epi32(rrprime_7, u64_magic_res_x10E8_7);
      rrprime_8 = _mm512_sub_epi32(rrprime_8, u64_magic_res_x10E8_8);
      rrprime_9 = _mm512_sub_epi32(rrprime_9, u64_magic_res_x10E8_9);
      rrprime_10 = _mm512_sub_epi32(rrprime_10, u64_magic_res_x10E8_10);
      rrprime_11 = _mm512_sub_epi32(rrprime_11, u64_magic_res_x10E8_11);
      rrprime_12 = _mm512_sub_epi32(rrprime_12, u64_magic_res_x10E8_12);
    }

    {
      const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
      const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
      const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
      const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);
      const __m256i u32_pack_5 = _mm512_cvtepi64_epi32(rrprime_5);
      const __m256i u32_pack_6 = _mm512_cvtepi64_epi32(rrprime_6);
      const __m256i u32_pack_7 = _mm512_cvtepi64_epi32(rrprime_7);
      const __m256i u32_pack_8 = _mm512_cvtepi64_epi32(rrprime_8);
      const __m256i u32_pack_9 = _mm512_cvtepi64_epi32(rrprime_9);
      const __m256i u32_pack_10 = _mm512_cvtepi64_epi32(rrprime_10);
      const __m256i u32_pack_11 = _mm512_cvtepi64_epi32(rrprime_11);
      const __m256i u32_pack_12 = _mm512_cvtepi64_epi32(rrprime_12);

      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), u32_pack_1);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), u32_pack_2);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), u32_pack_3);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), u32_pack_4);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[32]), u32_pack_5);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[40]), u32_pack_6);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[48]), u32_pack_7);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[56]), u32_pack_8);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[64]), u32_pack_9);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[72]), u32_pack_10);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[80]), u32_pack_11);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[88]), u32_pack_12);
    }
    //
  }
#elif defined(__AVX2__)
  void PositiveExponent(std::array<unsigned, MAX_ARRAY_SIZE> &SIMD_ARRAY, const int &exponent)
  {
    const auto &POW_2_E = Exponent::Positive::POW_2_E;
    const auto &POW_2_CACHE = Exponent::Positive::POW_2_CACHE;

    const unsigned K_DIV_64 = static_cast<unsigned>(exponent) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 40;
    const unsigned E_0 = POW_2_E[K_DIV_64];

    const __m256i ZERO = _mm256_setzero_si256();

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 4])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 8])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 12])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 16])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 20])));
    __m256i rrprime_7 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 24])));
    __m256i rrprime_8 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 28])));
    __m256i rrprime_9 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 32])));
    __m256i rrprime_10 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_2_CACHE[BASE_IDX + 36])));

    const __m256i R_10E8 = _mm256_set1_epi64x(100'000'000U);
    const __m256i R_MAGIC_10E8 = _mm256_set1_epi64x(1'441'151'881U);

    int e = E_0;
    for(; e + 5 < exponent; e += 5) // each iteration has ~25 cycle latency --- since 8 iterations is worst case then 200 cycle latency just in this loop ...
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

    int missing = exponent - e;

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

    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[0]), u32_pack_1);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[4]), u32_pack_2);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[8]), u32_pack_3);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[12]), u32_pack_4);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[16]), u32_pack_5);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[20]), u32_pack_6);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[24]), u32_pack_7);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[28]), u32_pack_8);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[32]), u32_pack_9);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[36]), u32_pack_10);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[40]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[48]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[56]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[64]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[72]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[80]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[88]), ZERO);
  }

  void NegativeExponent(std::array<unsigned, MAX_ARRAY_SIZE> &SIMD_ARRAY, const int &exponent)
  {
    const auto &POW_5_E = Exponent::Negative::POW_5_E;
    const auto &POW_5_CACHE = Exponent::Negative::POW_5_CACHE;

    const unsigned K_DIV_64 = static_cast<unsigned>(exponent) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 96;
    const unsigned E_0 = POW_5_E[K_DIV_64];

    const __m256i ZERO = _mm256_setzero_si256();

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
    __m256i rrprime_11 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 40])));
    __m256i rrprime_12 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 44])));
    __m256i rrprime_13 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 48])));
    __m256i rrprime_14 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 52])));
    __m256i rrprime_15 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 56])));
    __m256i rrprime_16 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 60])));
    __m256i rrprime_17 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 64])));
    __m256i rrprime_18 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 68])));
    __m256i rrprime_19 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 72])));
    __m256i rrprime_20 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 76])));
    __m256i rrprime_21 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 80])));
    __m256i rrprime_22 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 84])));
    __m256i rrprime_23 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 88])));
    __m256i rrprime_24 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[BASE_IDX + 92])));

    const __m256i R_25 = _mm256_set1_epi64x(25U);
    const __m256i R_10E8 = _mm256_set1_epi64x(100'000'000U);
    const __m256i R_MAGIC_10E8 = _mm256_set1_epi64x(1'441'151'881U);

    for(int e = E_0 + 2; e <= exponent; e += 2) // each iteration has ~74 cycle latency --- since 32 iterations is worst case then 2368 cycle latency just in this loop ...
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R_25);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R_25);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R_25);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R_25);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R_25);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R_25);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R_25);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R_25);
      const __m256i u64_prod_9 = _mm256_mul_epu32(rrprime_9, R_25);
      const __m256i u64_prod_10 = _mm256_mul_epu32(rrprime_10, R_25);
      const __m256i u64_prod_11 = _mm256_mul_epu32(rrprime_11, R_25);
      const __m256i u64_prod_12 = _mm256_mul_epu32(rrprime_12, R_25);
      const __m256i u64_prod_13 = _mm256_mul_epu32(rrprime_13, R_25);
      const __m256i u64_prod_14 = _mm256_mul_epu32(rrprime_14, R_25);
      const __m256i u64_prod_15 = _mm256_mul_epu32(rrprime_15, R_25);
      const __m256i u64_prod_16 = _mm256_mul_epu32(rrprime_16, R_25);
      const __m256i u64_prod_17 = _mm256_mul_epu32(rrprime_17, R_25);
      const __m256i u64_prod_18 = _mm256_mul_epu32(rrprime_18, R_25);
      const __m256i u64_prod_19 = _mm256_mul_epu32(rrprime_19, R_25);
      const __m256i u64_prod_20 = _mm256_mul_epu32(rrprime_20, R_25);
      const __m256i u64_prod_21 = _mm256_mul_epu32(rrprime_21, R_25);
      const __m256i u64_prod_22 = _mm256_mul_epu32(rrprime_22, R_25);
      const __m256i u64_prod_23 = _mm256_mul_epu32(rrprime_23, R_25);
      const __m256i u64_prod_24 = _mm256_mul_epu32(rrprime_24, R_25);
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
      const __m256i u64_magic_prod_11 = _mm256_mul_epu32(u64_prod_11, R_MAGIC_10E8);
      const __m256i u64_magic_prod_12 = _mm256_mul_epu32(u64_prod_12, R_MAGIC_10E8);
      const __m256i u64_magic_prod_13 = _mm256_mul_epu32(u64_prod_13, R_MAGIC_10E8);
      const __m256i u64_magic_prod_14 = _mm256_mul_epu32(u64_prod_14, R_MAGIC_10E8);
      const __m256i u64_magic_prod_15 = _mm256_mul_epu32(u64_prod_15, R_MAGIC_10E8);
      const __m256i u64_magic_prod_16 = _mm256_mul_epu32(u64_prod_16, R_MAGIC_10E8);
      const __m256i u64_magic_prod_17 = _mm256_mul_epu32(u64_prod_17, R_MAGIC_10E8);
      const __m256i u64_magic_prod_18 = _mm256_mul_epu32(u64_prod_18, R_MAGIC_10E8);
      const __m256i u64_magic_prod_19 = _mm256_mul_epu32(u64_prod_19, R_MAGIC_10E8);
      const __m256i u64_magic_prod_20 = _mm256_mul_epu32(u64_prod_20, R_MAGIC_10E8);
      const __m256i u64_magic_prod_21 = _mm256_mul_epu32(u64_prod_21, R_MAGIC_10E8);
      const __m256i u64_magic_prod_22 = _mm256_mul_epu32(u64_prod_22, R_MAGIC_10E8);
      const __m256i u64_magic_prod_23 = _mm256_mul_epu32(u64_prod_23, R_MAGIC_10E8);
      const __m256i u64_magic_prod_24 = _mm256_mul_epu32(u64_prod_24, R_MAGIC_10E8);
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
      const __m256i u64_magic_res_11 = _mm256_srli_epi64(u64_magic_prod_11, 57U);
      const __m256i u64_magic_res_12 = _mm256_srli_epi64(u64_magic_prod_12, 57U);
      const __m256i u64_magic_res_13 = _mm256_srli_epi64(u64_magic_prod_13, 57U);
      const __m256i u64_magic_res_14 = _mm256_srli_epi64(u64_magic_prod_14, 57U);
      const __m256i u64_magic_res_15 = _mm256_srli_epi64(u64_magic_prod_15, 57U);
      const __m256i u64_magic_res_16 = _mm256_srli_epi64(u64_magic_prod_16, 57U);
      const __m256i u64_magic_res_17 = _mm256_srli_epi64(u64_magic_prod_17, 57U);
      const __m256i u64_magic_res_18 = _mm256_srli_epi64(u64_magic_prod_18, 57U);
      const __m256i u64_magic_res_19 = _mm256_srli_epi64(u64_magic_prod_19, 57U);
      const __m256i u64_magic_res_20 = _mm256_srli_epi64(u64_magic_prod_20, 57U);
      const __m256i u64_magic_res_21 = _mm256_srli_epi64(u64_magic_prod_21, 57U);
      const __m256i u64_magic_res_22 = _mm256_srli_epi64(u64_magic_prod_22, 57U);
      const __m256i u64_magic_res_23 = _mm256_srli_epi64(u64_magic_prod_23, 57U);
      const __m256i u64_magic_res_24 = _mm256_srli_epi64(u64_magic_prod_24, 57U);
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
      const __m256i u64_magic_res_x10E8_11 = _mm256_mul_epu32(u64_magic_res_11, R_10E8);
      const __m256i u64_magic_res_x10E8_12 = _mm256_mul_epu32(u64_magic_res_12, R_10E8);
      const __m256i u64_magic_res_x10E8_13 = _mm256_mul_epu32(u64_magic_res_13, R_10E8);
      const __m256i u64_magic_res_x10E8_14 = _mm256_mul_epu32(u64_magic_res_14, R_10E8);
      const __m256i u64_magic_res_x10E8_15 = _mm256_mul_epu32(u64_magic_res_15, R_10E8);
      const __m256i u64_magic_res_x10E8_16 = _mm256_mul_epu32(u64_magic_res_16, R_10E8);
      const __m256i u64_magic_res_x10E8_17 = _mm256_mul_epu32(u64_magic_res_17, R_10E8);
      const __m256i u64_magic_res_x10E8_18 = _mm256_mul_epu32(u64_magic_res_18, R_10E8);
      const __m256i u64_magic_res_x10E8_19 = _mm256_mul_epu32(u64_magic_res_19, R_10E8);
      const __m256i u64_magic_res_x10E8_20 = _mm256_mul_epu32(u64_magic_res_20, R_10E8);
      const __m256i u64_magic_res_x10E8_21 = _mm256_mul_epu32(u64_magic_res_21, R_10E8);
      const __m256i u64_magic_res_x10E8_22 = _mm256_mul_epu32(u64_magic_res_22, R_10E8);
      const __m256i u64_magic_res_x10E8_23 = _mm256_mul_epu32(u64_magic_res_23, R_10E8);
      const __m256i u64_magic_res_x10E8_24 = _mm256_mul_epu32(u64_magic_res_24, R_10E8);

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
      const __m256i u32_hi_prod_perm_11 = _mm256_permute4x64_epi64(u64_magic_res_11, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_12 = _mm256_permute4x64_epi64(u64_magic_res_12, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_13 = _mm256_permute4x64_epi64(u64_magic_res_13, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_14 = _mm256_permute4x64_epi64(u64_magic_res_14, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_15 = _mm256_permute4x64_epi64(u64_magic_res_15, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_16 = _mm256_permute4x64_epi64(u64_magic_res_16, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_17 = _mm256_permute4x64_epi64(u64_magic_res_17, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_18 = _mm256_permute4x64_epi64(u64_magic_res_18, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_19 = _mm256_permute4x64_epi64(u64_magic_res_19, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_20 = _mm256_permute4x64_epi64(u64_magic_res_20, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_21 = _mm256_permute4x64_epi64(u64_magic_res_21, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_22 = _mm256_permute4x64_epi64(u64_magic_res_22, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_23 = _mm256_permute4x64_epi64(u64_magic_res_23, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_24 = _mm256_permute4x64_epi64(u64_magic_res_24, 0b10'01'00'11);
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
      const __m256i u64_magic_carry_slide_11 = _mm256_blend_epi32(u32_hi_prod_perm_11, u32_hi_prod_perm_10, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_12 = _mm256_blend_epi32(u32_hi_prod_perm_12, u32_hi_prod_perm_11, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_13 = _mm256_blend_epi32(u32_hi_prod_perm_13, u32_hi_prod_perm_12, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_14 = _mm256_blend_epi32(u32_hi_prod_perm_14, u32_hi_prod_perm_13, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_15 = _mm256_blend_epi32(u32_hi_prod_perm_15, u32_hi_prod_perm_14, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_16 = _mm256_blend_epi32(u32_hi_prod_perm_16, u32_hi_prod_perm_15, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_17 = _mm256_blend_epi32(u32_hi_prod_perm_17, u32_hi_prod_perm_16, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_18 = _mm256_blend_epi32(u32_hi_prod_perm_18, u32_hi_prod_perm_17, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_19 = _mm256_blend_epi32(u32_hi_prod_perm_19, u32_hi_prod_perm_18, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_20 = _mm256_blend_epi32(u32_hi_prod_perm_20, u32_hi_prod_perm_19, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_21 = _mm256_blend_epi32(u32_hi_prod_perm_21, u32_hi_prod_perm_20, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_22 = _mm256_blend_epi32(u32_hi_prod_perm_22, u32_hi_prod_perm_21, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_23 = _mm256_blend_epi32(u32_hi_prod_perm_23, u32_hi_prod_perm_22, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_24 = _mm256_blend_epi32(u32_hi_prod_perm_24, u32_hi_prod_perm_23, 0b00'00'00'11);

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
      rrprime_11 = _mm256_add_epi64(u64_prod_11, u64_magic_carry_slide_11);
      rrprime_12 = _mm256_add_epi64(u64_prod_12, u64_magic_carry_slide_12);
      rrprime_13 = _mm256_add_epi64(u64_prod_13, u64_magic_carry_slide_13);
      rrprime_14 = _mm256_add_epi64(u64_prod_14, u64_magic_carry_slide_14);
      rrprime_15 = _mm256_add_epi64(u64_prod_15, u64_magic_carry_slide_15);
      rrprime_16 = _mm256_add_epi64(u64_prod_16, u64_magic_carry_slide_16);
      rrprime_17 = _mm256_add_epi64(u64_prod_17, u64_magic_carry_slide_17);
      rrprime_18 = _mm256_add_epi64(u64_prod_18, u64_magic_carry_slide_18);
      rrprime_19 = _mm256_add_epi64(u64_prod_19, u64_magic_carry_slide_19);
      rrprime_20 = _mm256_add_epi64(u64_prod_20, u64_magic_carry_slide_20);
      rrprime_21 = _mm256_add_epi64(u64_prod_21, u64_magic_carry_slide_21);
      rrprime_22 = _mm256_add_epi64(u64_prod_22, u64_magic_carry_slide_22);
      rrprime_23 = _mm256_add_epi64(u64_prod_23, u64_magic_carry_slide_23);
      rrprime_24 = _mm256_add_epi64(u64_prod_24, u64_magic_carry_slide_24);

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
      rrprime_11 = _mm256_sub_epi32(rrprime_11, u64_magic_res_x10E8_11);
      rrprime_12 = _mm256_sub_epi32(rrprime_12, u64_magic_res_x10E8_12);
      rrprime_13 = _mm256_sub_epi32(rrprime_13, u64_magic_res_x10E8_13);
      rrprime_14 = _mm256_sub_epi32(rrprime_14, u64_magic_res_x10E8_14);
      rrprime_15 = _mm256_sub_epi32(rrprime_15, u64_magic_res_x10E8_15);
      rrprime_16 = _mm256_sub_epi32(rrprime_16, u64_magic_res_x10E8_16);
      rrprime_17 = _mm256_sub_epi32(rrprime_17, u64_magic_res_x10E8_17);
      rrprime_18 = _mm256_sub_epi32(rrprime_18, u64_magic_res_x10E8_18);
      rrprime_19 = _mm256_sub_epi32(rrprime_19, u64_magic_res_x10E8_19);
      rrprime_20 = _mm256_sub_epi32(rrprime_20, u64_magic_res_x10E8_20);
      rrprime_21 = _mm256_sub_epi32(rrprime_21, u64_magic_res_x10E8_21);
      rrprime_22 = _mm256_sub_epi32(rrprime_22, u64_magic_res_x10E8_22);
      rrprime_23 = _mm256_sub_epi32(rrprime_23, u64_magic_res_x10E8_23);
      rrprime_24 = _mm256_sub_epi32(rrprime_24, u64_magic_res_x10E8_24);
    }

    if((exponent & 0b1) != 0) // if its odd...
    {
      const __m256i R_5 = _mm256_set1_epi64x(5U);
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R_5);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R_5);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R_5);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R_5);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R_5);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R_5);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R_5);
      const __m256i u64_prod_8 = _mm256_mul_epu32(rrprime_8, R_5);
      const __m256i u64_prod_9 = _mm256_mul_epu32(rrprime_9, R_5);
      const __m256i u64_prod_10 = _mm256_mul_epu32(rrprime_10, R_5);
      const __m256i u64_prod_11 = _mm256_mul_epu32(rrprime_11, R_5);
      const __m256i u64_prod_12 = _mm256_mul_epu32(rrprime_12, R_5);
      const __m256i u64_prod_13 = _mm256_mul_epu32(rrprime_13, R_5);
      const __m256i u64_prod_14 = _mm256_mul_epu32(rrprime_14, R_5);
      const __m256i u64_prod_15 = _mm256_mul_epu32(rrprime_15, R_5);
      const __m256i u64_prod_16 = _mm256_mul_epu32(rrprime_16, R_5);
      const __m256i u64_prod_17 = _mm256_mul_epu32(rrprime_17, R_5);
      const __m256i u64_prod_18 = _mm256_mul_epu32(rrprime_18, R_5);
      const __m256i u64_prod_19 = _mm256_mul_epu32(rrprime_19, R_5);
      const __m256i u64_prod_20 = _mm256_mul_epu32(rrprime_20, R_5);
      const __m256i u64_prod_21 = _mm256_mul_epu32(rrprime_21, R_5);
      const __m256i u64_prod_22 = _mm256_mul_epu32(rrprime_22, R_5);
      const __m256i u64_prod_23 = _mm256_mul_epu32(rrprime_23, R_5);
      const __m256i u64_prod_24 = _mm256_mul_epu32(rrprime_24, R_5);
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
      const __m256i u64_magic_prod_11 = _mm256_mul_epu32(u64_prod_11, R_MAGIC_10E8);
      const __m256i u64_magic_prod_12 = _mm256_mul_epu32(u64_prod_12, R_MAGIC_10E8);
      const __m256i u64_magic_prod_13 = _mm256_mul_epu32(u64_prod_13, R_MAGIC_10E8);
      const __m256i u64_magic_prod_14 = _mm256_mul_epu32(u64_prod_14, R_MAGIC_10E8);
      const __m256i u64_magic_prod_15 = _mm256_mul_epu32(u64_prod_15, R_MAGIC_10E8);
      const __m256i u64_magic_prod_16 = _mm256_mul_epu32(u64_prod_16, R_MAGIC_10E8);
      const __m256i u64_magic_prod_17 = _mm256_mul_epu32(u64_prod_17, R_MAGIC_10E8);
      const __m256i u64_magic_prod_18 = _mm256_mul_epu32(u64_prod_18, R_MAGIC_10E8);
      const __m256i u64_magic_prod_19 = _mm256_mul_epu32(u64_prod_19, R_MAGIC_10E8);
      const __m256i u64_magic_prod_20 = _mm256_mul_epu32(u64_prod_20, R_MAGIC_10E8);
      const __m256i u64_magic_prod_21 = _mm256_mul_epu32(u64_prod_21, R_MAGIC_10E8);
      const __m256i u64_magic_prod_22 = _mm256_mul_epu32(u64_prod_22, R_MAGIC_10E8);
      const __m256i u64_magic_prod_23 = _mm256_mul_epu32(u64_prod_23, R_MAGIC_10E8);
      const __m256i u64_magic_prod_24 = _mm256_mul_epu32(u64_prod_24, R_MAGIC_10E8);
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
      const __m256i u64_magic_res_11 = _mm256_srli_epi64(u64_magic_prod_11, 57U);
      const __m256i u64_magic_res_12 = _mm256_srli_epi64(u64_magic_prod_12, 57U);
      const __m256i u64_magic_res_13 = _mm256_srli_epi64(u64_magic_prod_13, 57U);
      const __m256i u64_magic_res_14 = _mm256_srli_epi64(u64_magic_prod_14, 57U);
      const __m256i u64_magic_res_15 = _mm256_srli_epi64(u64_magic_prod_15, 57U);
      const __m256i u64_magic_res_16 = _mm256_srli_epi64(u64_magic_prod_16, 57U);
      const __m256i u64_magic_res_17 = _mm256_srli_epi64(u64_magic_prod_17, 57U);
      const __m256i u64_magic_res_18 = _mm256_srli_epi64(u64_magic_prod_18, 57U);
      const __m256i u64_magic_res_19 = _mm256_srli_epi64(u64_magic_prod_19, 57U);
      const __m256i u64_magic_res_20 = _mm256_srli_epi64(u64_magic_prod_20, 57U);
      const __m256i u64_magic_res_21 = _mm256_srli_epi64(u64_magic_prod_21, 57U);
      const __m256i u64_magic_res_22 = _mm256_srli_epi64(u64_magic_prod_22, 57U);
      const __m256i u64_magic_res_23 = _mm256_srli_epi64(u64_magic_prod_23, 57U);
      const __m256i u64_magic_res_24 = _mm256_srli_epi64(u64_magic_prod_24, 57U);
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
      const __m256i u64_magic_res_x10E8_11 = _mm256_mul_epu32(u64_magic_res_11, R_10E8);
      const __m256i u64_magic_res_x10E8_12 = _mm256_mul_epu32(u64_magic_res_12, R_10E8);
      const __m256i u64_magic_res_x10E8_13 = _mm256_mul_epu32(u64_magic_res_13, R_10E8);
      const __m256i u64_magic_res_x10E8_14 = _mm256_mul_epu32(u64_magic_res_14, R_10E8);
      const __m256i u64_magic_res_x10E8_15 = _mm256_mul_epu32(u64_magic_res_15, R_10E8);
      const __m256i u64_magic_res_x10E8_16 = _mm256_mul_epu32(u64_magic_res_16, R_10E8);
      const __m256i u64_magic_res_x10E8_17 = _mm256_mul_epu32(u64_magic_res_17, R_10E8);
      const __m256i u64_magic_res_x10E8_18 = _mm256_mul_epu32(u64_magic_res_18, R_10E8);
      const __m256i u64_magic_res_x10E8_19 = _mm256_mul_epu32(u64_magic_res_19, R_10E8);
      const __m256i u64_magic_res_x10E8_20 = _mm256_mul_epu32(u64_magic_res_20, R_10E8);
      const __m256i u64_magic_res_x10E8_21 = _mm256_mul_epu32(u64_magic_res_21, R_10E8);
      const __m256i u64_magic_res_x10E8_22 = _mm256_mul_epu32(u64_magic_res_22, R_10E8);
      const __m256i u64_magic_res_x10E8_23 = _mm256_mul_epu32(u64_magic_res_23, R_10E8);
      const __m256i u64_magic_res_x10E8_24 = _mm256_mul_epu32(u64_magic_res_24, R_10E8);

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
      const __m256i u32_hi_prod_perm_11 = _mm256_permute4x64_epi64(u64_magic_res_11, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_12 = _mm256_permute4x64_epi64(u64_magic_res_12, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_13 = _mm256_permute4x64_epi64(u64_magic_res_13, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_14 = _mm256_permute4x64_epi64(u64_magic_res_14, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_15 = _mm256_permute4x64_epi64(u64_magic_res_15, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_16 = _mm256_permute4x64_epi64(u64_magic_res_16, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_17 = _mm256_permute4x64_epi64(u64_magic_res_17, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_18 = _mm256_permute4x64_epi64(u64_magic_res_18, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_19 = _mm256_permute4x64_epi64(u64_magic_res_19, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_20 = _mm256_permute4x64_epi64(u64_magic_res_20, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_21 = _mm256_permute4x64_epi64(u64_magic_res_21, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_22 = _mm256_permute4x64_epi64(u64_magic_res_22, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_23 = _mm256_permute4x64_epi64(u64_magic_res_23, 0b10'01'00'11);
      const __m256i u32_hi_prod_perm_24 = _mm256_permute4x64_epi64(u64_magic_res_24, 0b10'01'00'11);
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
      const __m256i u64_magic_carry_slide_11 = _mm256_blend_epi32(u32_hi_prod_perm_11, u32_hi_prod_perm_10, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_12 = _mm256_blend_epi32(u32_hi_prod_perm_12, u32_hi_prod_perm_11, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_13 = _mm256_blend_epi32(u32_hi_prod_perm_13, u32_hi_prod_perm_12, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_14 = _mm256_blend_epi32(u32_hi_prod_perm_14, u32_hi_prod_perm_13, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_15 = _mm256_blend_epi32(u32_hi_prod_perm_15, u32_hi_prod_perm_14, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_16 = _mm256_blend_epi32(u32_hi_prod_perm_16, u32_hi_prod_perm_15, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_17 = _mm256_blend_epi32(u32_hi_prod_perm_17, u32_hi_prod_perm_16, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_18 = _mm256_blend_epi32(u32_hi_prod_perm_18, u32_hi_prod_perm_17, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_19 = _mm256_blend_epi32(u32_hi_prod_perm_19, u32_hi_prod_perm_18, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_20 = _mm256_blend_epi32(u32_hi_prod_perm_20, u32_hi_prod_perm_19, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_21 = _mm256_blend_epi32(u32_hi_prod_perm_21, u32_hi_prod_perm_20, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_22 = _mm256_blend_epi32(u32_hi_prod_perm_22, u32_hi_prod_perm_21, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_23 = _mm256_blend_epi32(u32_hi_prod_perm_23, u32_hi_prod_perm_22, 0b00'00'00'11);
      const __m256i u64_magic_carry_slide_24 = _mm256_blend_epi32(u32_hi_prod_perm_24, u32_hi_prod_perm_23, 0b00'00'00'11);

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
      rrprime_11 = _mm256_add_epi64(u64_prod_11, u64_magic_carry_slide_11);
      rrprime_12 = _mm256_add_epi64(u64_prod_12, u64_magic_carry_slide_12);
      rrprime_13 = _mm256_add_epi64(u64_prod_13, u64_magic_carry_slide_13);
      rrprime_14 = _mm256_add_epi64(u64_prod_14, u64_magic_carry_slide_14);
      rrprime_15 = _mm256_add_epi64(u64_prod_15, u64_magic_carry_slide_15);
      rrprime_16 = _mm256_add_epi64(u64_prod_16, u64_magic_carry_slide_16);
      rrprime_17 = _mm256_add_epi64(u64_prod_17, u64_magic_carry_slide_17);
      rrprime_18 = _mm256_add_epi64(u64_prod_18, u64_magic_carry_slide_18);
      rrprime_19 = _mm256_add_epi64(u64_prod_19, u64_magic_carry_slide_19);
      rrprime_20 = _mm256_add_epi64(u64_prod_20, u64_magic_carry_slide_20);
      rrprime_21 = _mm256_add_epi64(u64_prod_21, u64_magic_carry_slide_21);
      rrprime_22 = _mm256_add_epi64(u64_prod_22, u64_magic_carry_slide_22);
      rrprime_23 = _mm256_add_epi64(u64_prod_23, u64_magic_carry_slide_23);
      rrprime_24 = _mm256_add_epi64(u64_prod_24, u64_magic_carry_slide_24);

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
      rrprime_11 = _mm256_sub_epi32(rrprime_11, u64_magic_res_x10E8_11);
      rrprime_12 = _mm256_sub_epi32(rrprime_12, u64_magic_res_x10E8_12);
      rrprime_13 = _mm256_sub_epi32(rrprime_13, u64_magic_res_x10E8_13);
      rrprime_14 = _mm256_sub_epi32(rrprime_14, u64_magic_res_x10E8_14);
      rrprime_15 = _mm256_sub_epi32(rrprime_15, u64_magic_res_x10E8_15);
      rrprime_16 = _mm256_sub_epi32(rrprime_16, u64_magic_res_x10E8_16);
      rrprime_17 = _mm256_sub_epi32(rrprime_17, u64_magic_res_x10E8_17);
      rrprime_18 = _mm256_sub_epi32(rrprime_18, u64_magic_res_x10E8_18);
      rrprime_19 = _mm256_sub_epi32(rrprime_19, u64_magic_res_x10E8_19);
      rrprime_20 = _mm256_sub_epi32(rrprime_20, u64_magic_res_x10E8_20);
      rrprime_21 = _mm256_sub_epi32(rrprime_21, u64_magic_res_x10E8_21);
      rrprime_22 = _mm256_sub_epi32(rrprime_22, u64_magic_res_x10E8_22);
      rrprime_23 = _mm256_sub_epi32(rrprime_23, u64_magic_res_x10E8_23);
      rrprime_24 = _mm256_sub_epi32(rrprime_24, u64_magic_res_x10E8_24);
    }

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
    const __m128i u32_pack_11 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_11, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_12 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_12, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_13 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_13, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_14 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_14, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_15 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_15, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_16 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_16, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_17 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_17, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_18 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_18, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_19 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_19, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_20 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_20, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_21 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_21, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_22 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_22, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_23 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_23, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));
    const __m128i u32_pack_24 = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(rrprime_24, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7)));

    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[0]), u32_pack_1);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[4]), u32_pack_2);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[8]), u32_pack_3);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[12]), u32_pack_4);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[16]), u32_pack_5);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[20]), u32_pack_6);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[24]), u32_pack_7);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[28]), u32_pack_8);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[32]), u32_pack_9);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[36]), u32_pack_10);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[40]), u32_pack_11);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[44]), u32_pack_12);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[48]), u32_pack_13);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[52]), u32_pack_14);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[56]), u32_pack_15);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[60]), u32_pack_16);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[64]), u32_pack_17);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[68]), u32_pack_18);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[72]), u32_pack_19);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[76]), u32_pack_20);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[80]), u32_pack_21);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[84]), u32_pack_22);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[88]), u32_pack_23);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(&SIMD_ARRAY[92]), u32_pack_24);
  }

#else
#error "this algorithm is not supported for this architecture; this architecture is too old (pre __AVX2__)"
#endif

} // namespace Algos::Compute::DecimalExpansion
