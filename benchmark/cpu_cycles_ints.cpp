#if (defined(__GNUG__) || defined(__GNUC__)) && defined(__x86_64__) && (defined(__AVX2__) || defined(__AVX512F__))

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <limits>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include <boost/range/numeric.hpp>

#include <CJParse/include/CJParse.hpp>

#include "include/Algos/Integer.hpp"
#include "include/Benchmark/Measurements.hpp"
#include "include/Benchmark/PrintResults.hpp"
#include "include/Benchmark/StoreResults.hpp"
#include "include/Benchmark/SystemInfo.hpp"
#include "include/Helpers/Assembly.hpp"

#include <immintrin.h>

namespace
{
  enum class RETURN_TYPE
  {
    BUFFERED,
    STD_STRING
  };

  extern const char buffered[] = "buffered";
  extern const char std_string[] = "std_string";

  // Companion free function
  constexpr const char *to_string(RETURN_TYPE c)
  {
    switch(c)
    {
    case RETURN_TYPE::BUFFERED:
      return buffered;
    case RETURN_TYPE::STD_STRING:
      return std_string;
    default:
      return std_string;
    }
  }

  template <RETURN_TYPE RET, typename test_t>
    requires std::is_integral_v<test_t>
  void TestIntegralType()
  {
    Bin2Chars::Benchmark::Measurements::PmuTimer timer;

    constexpr size_t TRIALS = 100'000;
    constexpr size_t BATCH = 1'000;

    std::vector<uint64_t> simdy_tsc(TRIALS), simdy_clk(TRIALS), simdy_ref(TRIALS);
    std::vector<uint64_t> std_tsc(TRIALS), std_clk(TRIALS), std_ref(TRIALS);
    std::vector<uint64_t> empty_tsc(TRIALS), empty_clk(TRIALS), empty_ref(TRIALS);

    // 2. Pre-generate randoms to completely destroy std::to_string branch prediction
    std::vector<test_t> random_inputs(TRIALS * BATCH);
    std::mt19937 rng(123'456'678U);
    std::uniform_int_distribution<test_t> dist(0, static_cast<test_t>(std::numeric_limits<test_t>::max())); // Mix of digits
    for(auto &rand_in : random_inputs)
    {
      rand_in = dist(rng);
    }

    char buff[65];

    Bin2Chars::Helpers::Assembly::prefetch_elements<64>(&buff[0]);

    // 3. The Measurement Loop
    for(size_t i = 0; i < TRIALS; i++)
    {
      if constexpr(RET == RETURN_TYPE::BUFFERED)
      {
        _mm_clflush(buff);
        _mm_mfence();
        Bin2Chars::Helpers::Assembly::prefetch_elements<64>(&buff[0]);
      }

      const auto begin_simd = timer.start();

      for(size_t j = 0, idx = i * BATCH; j < BATCH; j++, idx++)
      {
        if constexpr(RET == RETURN_TYPE::BUFFERED)
        {
          const auto current_num = random_inputs[idx];
          const auto len = Bin2Chars::Numeric::Integral::ToStrBufferedReturnLen(&buff[0], current_num);
          // Force compiler to materialize the result
          asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "r"(len) : "memory");
        }
        else if constexpr(RET == RETURN_TYPE::STD_STRING)
        {
          const auto current_num = random_inputs[idx];
          const auto bin2chars = Bin2Chars::Numeric::Integral::ToStr(current_num);
          // Force compiler to materialize the result
          asm volatile("" : : "r"(bin2chars.data()) : "memory");
        }
      }

      const auto end_simdy = timer.stop();

      const auto delta_simdy = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin_simd, end_simdy);

      simdy_tsc[i] = delta_simdy.tsc_ticks;
      simdy_clk[i] = delta_simdy.clk_cycles;
      simdy_ref[i] = delta_simdy.ref_cycles;

      if constexpr(RET == RETURN_TYPE::BUFFERED)
      {
        _mm_clflush(buff);
        _mm_mfence();
        Bin2Chars::Helpers::Assembly::prefetch_elements<64>(&buff[0]);
      }

      const auto begin_std = timer.start();

      for(size_t j = 0, idx = i * BATCH; j < BATCH; j++, idx++)
      {
        if constexpr(RET == RETURN_TYPE::BUFFERED)
        {
          const auto current_num = random_inputs[idx];
          const auto len = std::to_chars(&buff[0], &buff[64], current_num);
          // Force compiler to materialize the result
          asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "r"(len) : "memory");
        }
        else if constexpr(RET == RETURN_TYPE::STD_STRING)
        {
          const auto current_num = random_inputs[idx];
          const auto stdlib = std::to_string(current_num);
          // Force compiler to materialize the result
          asm volatile("" : : "r"(stdlib.data()) : "memory");
        }
      }

      const auto end_std = timer.stop();

      const auto delta_std = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin_std, end_std);

      std_tsc[i] = delta_std.tsc_ticks;
      std_clk[i] = delta_std.clk_cycles;
      std_ref[i] = delta_std.ref_cycles;

      const auto begin_empty = timer.start();

      for(size_t j = 0; j < BATCH; ++j)
      {
        asm volatile("" ::: "memory");
      }

      const auto end_empty = timer.stop();

      const auto delta_empty = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin_empty, end_empty);

      empty_tsc[i] = delta_empty.tsc_ticks;
      empty_clk[i] = delta_empty.clk_cycles;
      empty_ref[i] = delta_empty.ref_cycles;

      std::this_thread::yield(); // so timer interrups 'can' (big enphasis on CAN) happen here instead of in the middle of measurements (hopefully)
    }

    CJParse::CJParse json{ "null" };
    json.JSON = CJParse::Types::Object{};

    Bin2Chars::Benchmark::PrintPmuResults(
        test_t{}, json.JSON, "return <" + std::string(to_string(RET)) + "> - itoa", 0, BATCH,
        Bin2Chars::Benchmark::PmuResult{
            .label = "BIN2CHARS", .tsc = simdy_tsc, .core = simdy_clk, .ref = simdy_ref, .empty_tsc = empty_tsc, .empty_core = empty_clk, .empty_ref = empty_ref },
        Bin2Chars::Benchmark::PmuResult{
            .label = "STD_LIB", .tsc = std_tsc, .core = std_clk, .ref = std_ref, .empty_tsc = empty_tsc, .empty_core = empty_clk, .empty_ref = empty_ref });

    Bin2Chars::Benchmark::Store::File file{};

    file.Store<to_string(RET), test_t>(json, false, 0);
  }
} // namespace

int main(int /*unused*/, char ** /*unused*/)
{
  try
  {
    Bin2Chars::Benchmark::SystemInfo::cpu_id = 0;
    // 1. Pin to a specific core to avoid cross-core TSC sync issues
    Bin2Chars::Helpers::Assembly::pin_thread_to_cpu(Bin2Chars::Benchmark::SystemInfo::cpu_id);

    TestIntegralType<RETURN_TYPE::BUFFERED, int8_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, uint8_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, int16_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, uint16_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, int32_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, uint32_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, int64_t>();
    TestIntegralType<RETURN_TYPE::BUFFERED, uint64_t>();

    TestIntegralType<RETURN_TYPE::STD_STRING, int8_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, uint8_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, int16_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, uint16_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, int32_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, uint32_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, int64_t>();
    TestIntegralType<RETURN_TYPE::STD_STRING, uint64_t>();
  }
  catch(std::exception &exept)
  {
    std::cerr << exept.what() << std::endl;
  }
}

#else

int main()
{
  return 0;
}

#endif

#undef BIN2CHARS_ALWAYS_INLINE
