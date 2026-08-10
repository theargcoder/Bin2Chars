#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <string>

#define ALL_DIGITS_CALCULATED_NO_LOOP_EXTRACTION_NEEDED 1

// this activates the algorithms that calculate with FULL precicion the 5^k expansion
// this needs 96 unsigned bytes to be stored once completed

#if ALL_DIGITS_CALCULATED_NO_LOOP_EXTRACTION_NEEDED != 0
#if defined(__AVX512BW__) && defined(__AVX512VL__)

int main()
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

  for(int k = 0; k < 1075; k++)
  {
  backwards:
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));
    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;

    constexpr unsigned NUM_WORDS = 96;

    const unsigned K_DIV_8 = static_cast<unsigned>(k) >> 3U;
    const unsigned K_DIV_64 = static_cast<unsigned>(k) >> 6U;
    const unsigned BASE_IDX = K_DIV_64 * 96;
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

    for(int e = E_0 + 2; e <= k; e += 2) // each iteration has ~74 cycle latency --- since 32 iterations is worst case then 2368 cycle latency just in this loop ...
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

    if((k & 0b1) != 0) // if its odd...
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

    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), rrprime_2);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), rrprime_3);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), rrprime_4);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[32]), rrprime_5);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[40]), rrprime_6);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[48]), rrprime_7);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[56]), rrprime_8);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[64]), rrprime_9);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[72]), rrprime_10);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[80]), rrprime_11);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[88]), rrprime_12);

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

      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_2);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_3);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_4);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[32]), u32_pack_5);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[40]), u32_pack_6);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[48]), u32_pack_7);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[56]), u32_pack_8);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[64]), u32_pack_9);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[72]), u32_pack_10);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[80]), u32_pack_11);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[88]), u32_pack_12);
    }

    SLOW_PRE_ARR[0] = 1; // Initialize 5^0 = 1

    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    // Main loop: Process in steps of 5^2 = 25 (halves total loop iterations)
    const uint32_t pairs = k / 2; // 537 passes of x25
    for(uint32_t i = 0; i < pairs; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        // Max pp = 99,999,999 * 25 + 24 = 2,499,999,999 (fits safely in 32-bit uint)
        uint64_t pp = (uint64_t)w * 25U + carry;

        // Fast division by 10^8
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);

        // Fast remainder: pp % 10^8
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    // Handle odd exponent leftover (5^1)
    if(k % 2 != 0)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 5U + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
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
      std::cout << "k = " << k << "; p = " << P << '\n';
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
}

#else
#error "this algorithm is not supported for this architecture; this architecture is too old (pre __AVX2__)"
#endif

// this avoids the algorithm that calculate with FULL precicion the 5^k expansion by computing 10^k and then
// extracting it in a LOOP using MOD 2^32
// this needs 40 unsigned bytes to be stored once completed; thus reducing SIMD pressure
// and other nice things that may make it a a more actractive alternative

#else
#if defined(__AVX512BW__) && defined(__AVX512VL__)

int main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

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
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m512i ZERO = _mm512_setzero_si512();

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 33> SIMD_ARRAY;
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), ZERO);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), ZERO);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), ZERO);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), ZERO);

    __m512i rrprime_1 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m512i rrprime_2 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m512i rrprime_3 = _mm512_cvtepu32_epi64(_mm256_loadu_si256(reinterpret_cast<const __m256i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m512i rrprime_4 = ZERO;
    const __m512i R1_E8 = _mm512_set1_epi64(POW_5_CORRECTION[8]);
    const __m512i R1_REMMISSING = _mm512_set1_epi64(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned E_0 = pow_5_e[P_DIV_32];

    for(unsigned e = E_0; e < P_DIV_8; e++)
    {
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, R1_E8);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, R1_E8);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, R1_E8);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, R1_E8);
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
      const __m512i u64_prod_1 = _mm512_mul_epu32(rrprime_1, R1_REMMISSING);
      const __m512i u64_prod_2 = _mm512_mul_epu32(rrprime_2, R1_REMMISSING);
      const __m512i u64_prod_3 = _mm512_mul_epu32(rrprime_3, R1_REMMISSING);
      const __m512i u64_prod_4 = _mm512_mul_epu32(rrprime_4, R1_REMMISSING);
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

    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), rrprime_1);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), rrprime_2);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), rrprime_3);
    _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), rrprime_4);

    {
      const __m256i u32_pack_1 = _mm512_cvtepi64_epi32(rrprime_1);
      const __m256i u32_pack_2 = _mm512_cvtepi64_epi32(rrprime_2);
      const __m256i u32_pack_3 = _mm512_cvtepi64_epi32(rrprime_3);
      const __m256i u32_pack_4 = _mm512_cvtepi64_epi32(rrprime_4);

      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[0]), u32_pack_1);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[8]), u32_pack_2);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[16]), u32_pack_3);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_PRE_WRITTEN[24]), u32_pack_4);
    }

    SLOW_PRE_ARR[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 10ull + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = SLOW_PRE_ARR;

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
      std::cout << "k = " << k << "; p = " << P << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    // unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    unsigned simdy_idx = (K_DIV_32 - ((k * 1233U) >> 17U));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t pp = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }

      {
        __m512i rrrprime_1 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]));
        __m512i rrrprime_2 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]));
        __m512i rrrprime_3 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]));
        __m512i rrrprime_4 = _mm512_loadu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]));
        const __m512i u64_prod_1 = _mm512_mul_epu32(rrrprime_1, R1_E8);
        const __m512i u64_prod_2 = _mm512_mul_epu32(rrrprime_2, R1_E8);
        const __m512i u64_prod_3 = _mm512_mul_epu32(rrrprime_3, R1_E8);
        const __m512i u64_prod_4 = _mm512_mul_epu32(rrrprime_4, R1_E8);
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
        rrrprime_1 = _mm512_add_epi64(u32_lo_prod_1, u32_hi_prod_slide_1);
        rrrprime_2 = _mm512_add_epi64(u32_lo_prod_2, u32_hi_prod_slide_2);
        rrrprime_3 = _mm512_add_epi64(u32_lo_prod_3, u32_hi_prod_slide_3);
        rrrprime_4 = _mm512_add_epi64(u32_lo_prod_4, u32_hi_prod_slide_4);
        const __m512i rrprime_slide_1 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_1, ZERO, 7), 32);
        const __m512i rrprime_slide_2 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_2, rrrprime_1, 7), 32);
        const __m512i rrprime_slide_3 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_3, rrrprime_2, 7), 32);
        const __m512i rrprime_slide_4 = _mm512_srli_epi64(_mm512_alignr_epi64(rrrprime_4, rrrprime_3, 7), 32);
        rrrprime_1 = _mm512_add_epi32(rrrprime_1, rrprime_slide_1);
        rrrprime_2 = _mm512_add_epi32(rrrprime_2, rrprime_slide_2);
        rrrprime_3 = _mm512_add_epi32(rrrprime_3, rrprime_slide_3);
        rrrprime_4 = _mm512_add_epi32(rrrprime_4, rrprime_slide_4);
        rrrprime_1 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_1, ZERO);
        rrrprime_2 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_2, ZERO);
        rrrprime_3 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_3, ZERO);
        rrrprime_4 = _mm512_mask_blend_epi32(0b1010'1010'1010'1010, rrrprime_4, ZERO);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[0]), rrrprime_1);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[8]), rrrprime_2);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[16]), rrrprime_3);
        _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[24]), rrrprime_4);
      }

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      chunk = SIMD_ARRAY[simdy_idx] >> bit_shift;
      chunk |= (SIMD_ARRAY[simdy_idx + 1] << (32 - bit_shift));
      simdy_8 = (uint32_t)chunk;
      SIMD_ARRAY[simdy_idx] &= mod_mask;
      _mm512_storeu_si512(reinterpret_cast<__m512i *>(&SIMD_ARRAY[simdy_idx + 1]), ZERO);

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; p = " << P << '\n';
        goto backwards;
      }

      assert(simdy_8 == next_8);

      // step e: print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }
    //
  }

  return 0;
}

#elif defined(__AVX2__)

int __main()
{
  constexpr uint32_t POW_5_CORRECTION[] = { 1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000 };
  constexpr uint8_t POW_5_IDX[] = { 0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240 };
  constexpr uint8_t pow_5_e[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40 };

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
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    const unsigned P_DIV_8 = P >> 3U;

    const unsigned P_DIV_32 = P >> 5U;

    const unsigned K_DIV_32 = static_cast<unsigned>(k) >> 5U;

    const __m256i ZERO = _mm256_setzero_si256();

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 40;
    std::array<uint32_t, NUM_WORDS> SIMD_PRE_WRITTEN = { 0 };
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    std::array<uint64_t, 29> SIMD_ARRAY; // 29 to avoid UB when word_idx + 1 in simd mod write 0
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]), ZERO);
    _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]), ZERO);
    SIMD_ARRAY[28] = 0;

    __m256i rrprime_1 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32]])));
    __m256i rrprime_2 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 4U])));
    __m256i rrprime_3 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 8U])));
    __m256i rrprime_4 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 12U])));
    __m256i rrprime_5 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 16U])));
    __m256i rrprime_6 = _mm256_cvtepu32_epi64(_mm_loadu_si128(reinterpret_cast<const __m128i *>(&POW_5_CACHE[POW_5_IDX[P_DIV_32] + 20U])));
    __m256i rrprime_7 = ZERO;
    const __m256i R1_E8 = _mm256_set1_epi64x(POW_5_CORRECTION[8]);
    const __m256i R1_REMMISSING = _mm256_set1_epi64x(POW_5_CORRECTION[P - (P_DIV_8 << 3U)]);

    const unsigned e_o = pow_5_e[P_DIV_32];

    for(unsigned e = e_o; e < P_DIV_8; e++)
    {
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1_E8);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1_E8);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1_E8);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1_E8);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1_E8);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1_E8);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1_E8);
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
      const __m256i u64_prod_1 = _mm256_mul_epu32(rrprime_1, R1_REMMISSING);
      const __m256i u64_prod_2 = _mm256_mul_epu32(rrprime_2, R1_REMMISSING);
      const __m256i u64_prod_3 = _mm256_mul_epu32(rrprime_3, R1_REMMISSING);
      const __m256i u64_prod_4 = _mm256_mul_epu32(rrprime_4, R1_REMMISSING);
      const __m256i u64_prod_5 = _mm256_mul_epu32(rrprime_5, R1_REMMISSING);
      const __m256i u64_prod_6 = _mm256_mul_epu32(rrprime_6, R1_REMMISSING);
      const __m256i u64_prod_7 = _mm256_mul_epu32(rrprime_7, R1_REMMISSING);
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

    SLOW_PRE_ARR[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 10ull + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = SLOW_PRE_ARR;

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
      std::cout << "k = " << k << "; p = " << P << '\n';
      goto backwards;
    }

    assert(SIMD_PRE_WRITTEN == SLOW_PRE_ARR);

    // unsigned lead_z = 24u - (k_div_32 - ((k * 1233u) >> 17u)) - (k < 32); //+ (k_div_32 & 0b0111);

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32u;
    unsigned simdy_idx = (K_DIV_32 - ((k * 1233U) >> 17U));
    uint64_t chunk = 0;
    unsigned next_8;
    unsigned simdy_8;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t pp = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32);
      }

      {
        __m256i rrrprime_1 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[0]));
        __m256i rrrprime_2 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[4]));
        __m256i rrrprime_3 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[8]));
        __m256i rrrprime_4 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[12]));
        __m256i rrrprime_5 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[16]));
        __m256i rrrprime_6 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[20]));
        __m256i rrrprime_7 = _mm256_loadu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[24]));
        const __m256i u64_prod_1 = _mm256_mul_epu32(rrrprime_1, R1_E8);
        const __m256i u64_prod_2 = _mm256_mul_epu32(rrrprime_2, R1_E8);
        const __m256i u64_prod_3 = _mm256_mul_epu32(rrrprime_3, R1_E8);
        const __m256i u64_prod_4 = _mm256_mul_epu32(rrrprime_4, R1_E8);
        const __m256i u64_prod_5 = _mm256_mul_epu32(rrrprime_5, R1_E8);
        const __m256i u64_prod_6 = _mm256_mul_epu32(rrrprime_6, R1_E8);
        const __m256i u64_prod_7 = _mm256_mul_epu32(rrrprime_7, R1_E8);
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

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      chunk = SIMD_ARRAY[simdy_idx] >> bit_shift;
      chunk |= (SIMD_ARRAY[simdy_idx + 1] << (32 - bit_shift));
      simdy_8 = (uint32_t)chunk;
      SIMD_ARRAY[simdy_idx] &= mod_mask;
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(&SIMD_ARRAY[simdy_idx + 1]), ZERO);

      if(simdy_8 != next_8)
      {
        std::cout << "k = " << k << "; p = " << P << '\n';
        goto backwards;
      }

      assert(simdy_8 == next_8);

      // step e: print immediately
      // std::cout << std::setfill('0') << std::setw(8) << simdy_8;

      digits_computed += 8;
    }
    //
  }

  return 0;
}

#else
#error "this algorithm is not supported for this architecture; this architecture is too old (pre __AVX2__)"
#endif
#endif

int printer_arr()
{
  std::cout << " constexpr uint8_t POW_5_E[] = {";
  for(unsigned k = 0; k < 1075; k++)
  {
    if(k % 64 != 0)
    {
      continue;
    }

    std::cout << k << ", ";
  }
  std::cout << "\b}\n";

  std::cout << " constexpr uint32_t POW_5_CACHE[] = { \n";
  for(unsigned k = 0; k < 1075; k++)
  {
    if(k % 16 != 0)
    {
      continue;
    }

    constexpr unsigned NUM_WORDS = 96;
    std::array<uint32_t, NUM_WORDS> OTHER_PRE_ARR = { 0 };

    OTHER_PRE_ARR[0] = 1; // Initialize 5^0 = 1

    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    // Main loop: Process in steps of 5^2 = 25 (halves total loop iterations)
    const uint32_t pairs = k / 2; // 537 passes of x25
    for(uint32_t i = 0; i < pairs; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : OTHER_PRE_ARR)
      {
        // Max pp = 99,999,999 * 25 + 24 = 2,499,999,999 (fits safely in 32-bit uint)
        uint64_t pp = (uint64_t)w * 25U + carry;

        // Fast division by 10^8
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);

        // Fast remainder: pp % 10^8
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    // Handle odd exponent leftover (5^1)
    if(k % 2 != 0)
    {
      uint32_t carry = 0;
      for(unsigned int &w : OTHER_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 5U + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
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
      std::cout << OTHER_PRE_ARR[w] << ", ";
      xx++;
    }
    std::cout << " // " << yy << " - anotherone \n";
    std::cout << "\n";
  }
  std::cout << "\b};\n";

  return 0;
}

int test_proposal_vs_modified()
{
  for(int k = 0; k < 1075; k++)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 96;
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };
    std::array<uint32_t, NUM_WORDS> OTHER_PRE_ARR = { 0 };

    SLOW_PRE_ARR[0] = 1;  // initialize r = 1
    OTHER_PRE_ARR[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 10ULL + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32U);
      }
    }

    constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
    constexpr int SHIFT_10E8 = 57;

    // Main loop: Process in steps of 5^2 = 25 (halves total loop iterations)
    const uint32_t pairs = k / 2; // 537 passes of x25
    for(uint32_t i = 0; i < pairs; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : OTHER_PRE_ARR)
      {
        // Max pp = 99,999,999 * 25 + 24 = 2,499,999,999 (fits safely in 32-bit uint)
        uint64_t pp = (uint64_t)w * 25U + carry;

        // Fast division by 10^8
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);

        // Fast remainder: pp % 10^8
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    // Handle odd exponent leftover (5^1)
    if(k % 2 != 0)
    {
      uint32_t carry = 0;
      for(unsigned int &w : OTHER_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 5U + carry;
        carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
        w = (uint32_t)(pp - carry * 100'000'000U);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = SLOW_PRE_ARR;

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

    // Find most significant non-zero chunk
    int top_word = NUM_WORDS - 1;
    while(top_word > 0 && OTHER_PRE_ARR[top_word] == 0)
    {
      --top_word;
    }

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32U;
    uint64_t chunk = 0;
    unsigned next_8 = 0;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    std::string option_1, option_2;

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      char buf[9];
      std::snprintf(buf, sizeof(buf), "%08u", next_8);
      option_1 += buf;

      digits_computed += 8;
    }

    // Top word keeps its natural size, all lower words get 8-digit zero padding
    option_2 += std::to_string(OTHER_PRE_ARR[top_word]);
    for(int w = top_word - 1; w >= 0; --w)
    {
      char buf[9];
      std::snprintf(buf, sizeof(buf), "%08u", OTHER_PRE_ARR[w]);
      option_2 += buf;
    }

    assert(option_1.find(option_2) != std::string::npos);
  }
}

int better_proposal()
{
  constexpr uint32_t k = 1075;

  constexpr unsigned NUM_WORDS = 96;
  std::array<uint32_t, NUM_WORDS> OTHER_PRE_ARR = { 0 };

  OTHER_PRE_ARR[0] = 1; // Initialize 5^0 = 1

  constexpr uint64_t MAGIC_10E8 = 1441151881ULL;
  constexpr int SHIFT_10E8 = 57;

  // Main loop: Process in steps of 5^2 = 25 (halves total loop iterations)
  const uint32_t pairs = k / 2; // 537 passes of x25
  for(uint32_t i = 0; i < pairs; ++i)
  {
    uint32_t carry = 0;
    for(unsigned int &w : OTHER_PRE_ARR)
    {
      // Max pp = 99,999,999 * 25 + 24 = 2,499,999,999 (fits safely in 32-bit uint)
      uint64_t pp = (uint64_t)w * 25U + carry;

      // Fast division by 10^8
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);

      // Fast remainder: pp % 10^8
      w = (uint32_t)(pp - carry * 100'000'000U);
    }
  }

  // Handle odd exponent leftover (5^1)
  if(k % 2 != 0)
  {
    uint32_t carry = 0;
    for(unsigned int &w : OTHER_PRE_ARR)
    {
      uint64_t pp = (uint64_t)w * 5U + carry;
      carry = (uint32_t)((pp * MAGIC_10E8) >> SHIFT_10E8);
      w = (uint32_t)(pp - carry * 100'000'000U);
    }
  }

  // Find most significant non-zero chunk
  int top_word = NUM_WORDS - 1;
  while(top_word > 0 && OTHER_PRE_ARR[top_word] == 0)
  {
    --top_word;
  }

  // Output formatted result
  std::cout << OTHER_PRE_ARR[top_word]; // Unpadded leading chunk
  for(int w = top_word - 1; w >= 0; --w)
  {
    std::cout << std::setfill('0') << std::setw(8) << OTHER_PRE_ARR[w];
  }
  std::cout << "\n";

  return 0;
}

int modified_algo()
{
  for(int k = 0; k < 1075; k++)
  {
    // 36 words * 32 bits = 1152 bits. perfectly fits k=1100 + 10^8 overflow.
    const unsigned P = std::floor(k * std::log10(2));

    // total digits in 5^k
    const uint32_t TOTAL_DIGITS = std::floor(k * std::log10(5)) + 1;
    constexpr unsigned NUM_WORDS = 96;
    std::array<uint32_t, NUM_WORDS> SLOW_PRE_ARR = { 0 };

    SLOW_PRE_ARR[0] = 1; // initialize r = 1

    for(uint32_t i = 0; i < P; ++i)
    {
      uint32_t carry = 0;
      for(unsigned int &w : SLOW_PRE_ARR)
      {
        uint64_t pp = (uint64_t)w * 10ULL + carry;
        w = (uint32_t)pp;
        carry = (uint32_t)(pp >> 32U);
      }
    }

    std::array<uint32_t, NUM_WORDS> x10byx10_arr = SLOW_PRE_ARR;

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

    unsigned digits_computed = 0;
    unsigned word_idx = k / 32U;
    uint64_t chunk = 0;
    unsigned next_8 = 0;

    const unsigned bit_shift = k & ((1U << 5U) - 1);
    const unsigned mod_mask = (bit_shift == 0) ? 0 : ((1U << bit_shift) - 1);

    while(digits_computed < TOTAL_DIGITS)
    {
      // step a & b: multiply by 10^8 and cascade the carry
      uint32_t carry = 0; // carry max is 10^8, fits safely in uint32_t
      for(unsigned int &w : x10byx10_arr)
      {
        // max size: (2^32 - 1) * 10^8 + 10^8 = 2^32 * 10^8. fits comfortably in uint64_t.
        uint64_t p = (uint64_t)w * 100'000'000u + carry;
        w = (uint32_t)p;
        carry = (uint32_t)(p >> 32);
      }

      // step c: extract next8 chunk (r >> k)
      chunk = x10byx10_arr[word_idx] >> bit_shift;
      // cast to uint64_t before shifting to prevent any 32-bit boundary overflow
      chunk |= ((uint64_t)x10byx10_arr[word_idx + 1] << (32 - bit_shift));
      next_8 = (uint32_t)chunk;

      x10byx10_arr[word_idx] &= mod_mask;
      std::memset(static_cast<void *>(&x10byx10_arr[word_idx + 1]), 0, (NUM_WORDS - word_idx - 1) * 4);

      // step e: print immediately
      std::cout << std::setfill('0') << std::setw(8) << next_8;

      digits_computed += 8;
    }
    //
  }
}

int original_algo()
{
  uint32_t k = 128;

  // Total digits in 5^128
  uint32_t total_digits = std::floor(k * std::log10(5)) + 1; // 90 digits

  // Power of 10 needed for the first 8-digit chunk
  // uint32_t P = (std::floor(k * std::log10(2)) + 8); // For k=128, P=46
  uint32_t P = (std::floor(k * std::log10(2))); // For k=128, P=46

  // Representing a 192-bit fixed integer space using 32-bit limbs
  // r0 = bits 0-31, r1 = bits 32-63, r2 = bits 64-95,
  // r3 = bits 96-127, r4 = bits 128-159, r5 = bits 160-191
  uint32_t r0 = 1, r1 = 0, r2 = 0, r3 = 0;

  for(uint32_t i = 0; i < P; ++i)
  {
    // Use uint64_t for intermediate steps to safely capture the carry (overflow)
    uint64_t p0 = (uint64_t)r0 * 10;
    r0 = (uint32_t)p0;

    uint64_t p1 = (uint64_t)r1 * 10 + (p0 >> 32);
    r1 = (uint32_t)p1;

    uint64_t p2 = (uint64_t)r2 * 10 + (p1 >> 32);
    r2 = (uint32_t)p2;

    uint64_t p3 = (uint64_t)r3 * 10 + (p2 >> 32);
    r3 = (uint32_t)p3;
  }

  // Extract the first chunk (bits 160-191, which live in r5)
  uint32_t first_chunk = r3;

  std::cout << "Chunk 1: " << std::setfill('0') << std::setw(8) << first_chunk << "\n";

  // 2. Main Processing Loop: Dynamically compute chunks inline
  uint32_t digits_computed = 8;
  int chunk_count = 2;

  while(digits_computed < total_digits)
  {
    // Step A: Multiply the lower 64 bits by 10^8
    unsigned __int128 p0 = (unsigned __int128)r0 * 100000000ULL;
    r0 = (uint64_t)p0;

    // Step B: Multiply the upper 64 bits and add the overflow/carry from p0
    unsigned __int128 p1 = (unsigned __int128)r1 * 100000000ULL + (p0 >> 64);
    r1 = (uint64_t)p1;

    // Step C: Extract next8 chunk & Apply Modulo 2^128 instantly
    // The bits overflowing past bit 128 represent (R >> 128), captured perfectly by p1 >> 64
    uint32_t next8 = (uint32_t)(p1 >> 64);

    // Step D: Print immediately
    std::cout << "Chunk " << chunk_count++ << ": " << std::setfill('0') << std::setw(8) << next8 << "\n";

    digits_computed += 8;
  }

  return 0;
}
