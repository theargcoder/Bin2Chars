#if defined(__GNUC__) && defined(__x86_64__) && (defined(__AVX2__) || defined(__AVX512F__))

#include <charconv>
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

#include <ryu/ryu.h>

#include <CJParse/include/CJParse.hpp>

#include "include/Algos/Competition.hpp"
#include "include/Algos/Floating/DecimalExpansion.hpp"
#include "include/Algos/Floating/Exponential.hpp"
#include "include/Benchmark/Measurements.hpp"
#include "include/Benchmark/PrintResults.hpp"
#include "include/Benchmark/StoreResults.hpp"
#include "include/Benchmark/SystemInfo.hpp"
#include "include/Helpers/Assembly.hpp"

namespace
{
  enum class RETURN_TYPE
  {
    BUFFERED,
    STD_STRING
  };

  enum class FORMAT
  {
    EXPONENTIAL,
    DECIMAL
  };

  extern const char buffered[] = "buffered";
  extern const char std_string[] = "std_string";
  extern const char exponential[] = "exponential";
  extern const char decimal[] = "decimal";

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

  // Companion free function
  constexpr const char *to_string(FORMAT c)
  {
    switch(c)
    {
    case FORMAT::EXPONENTIAL:
      return exponential;
    case FORMAT::DECIMAL:
      return decimal;
    default:
      return exponential;
    }
  }

  template <size_t TRIALS, size_t BATCH, FORMAT FMT, RETURN_TYPE RET, typename test_t>
    requires std::is_floating_point_v<test_t>
  void TestFloatingType(CJParse::Types::JsonValue &json, const int &PRECISION)
  {
    Bin2Chars::Benchmark::Measurements::PmuTimer timer;

    using base_t = std::conditional_t<std::is_same_v<test_t, float>, uint32_t, uint64_t>;

    std::vector<uint64_t> simdy_tsc(TRIALS), simdy_clk(TRIALS), simdy_ref(TRIALS);
    std::vector<uint64_t> std_tsc(TRIALS), std_clk(TRIALS), std_ref(TRIALS);
    std::vector<uint64_t> ryu_tsc(TRIALS), ryu_clk(TRIALS), ryu_ref(TRIALS);
    std::vector<uint64_t> empty_tsc(TRIALS), empty_clk(TRIALS), empty_ref(TRIALS);

    // 2. Pre-generate randoms to completely destroy std::to_string branch prediction
    std::vector<test_t> random_inputs(TRIALS * BATCH);
    std::mt19937 rng(123'456'678U);
    std::uniform_int_distribution<base_t> dist(0, static_cast<base_t>(std::numeric_limits<base_t>::max())); // Mix of digits
    for(auto &rand_in : random_inputs)
    {
      rand_in = std::bit_cast<test_t>(dist(rng));
    }

    char buff[2049];

    Bin2Chars::Helpers::Assembly::prefetch_elements<2048>(&buff[0]);

    // 3. The Measurement Loop
    for(size_t i = 0; i < TRIALS; i++)
    {
      // Bin2Chars
      {
        if constexpr(RET == RETURN_TYPE::BUFFERED)
        {
          Bin2Chars::Helpers::Assembly::prefetch_elements<2048>(&buff[0]);
        }

        Bin2Chars::Helpers::Assembly::prefetch_elements<BATCH>(&random_inputs[i]);

        const auto begin = timer.start();

        for(size_t j = 0, idx = i * BATCH; j < BATCH; j++, idx++)
        {
          const auto current_num = random_inputs[idx];
          if constexpr(RET == RETURN_TYPE::BUFFERED)
          {
            unsigned len;
            if constexpr(FMT == FORMAT::EXPONENTIAL)
            {
              len = Bin2Chars::Numeric::Floating::ExponentialNotation::ToStrCharArray(&buff[0], current_num, PRECISION);
            }
            else if(FMT == FORMAT::DECIMAL)
            {
              len = Bin2Chars::Numeric::Floating::DigitsPrecision::ToStrWriteBuffReturnLen<test_t>(&buff[0], current_num, PRECISION);
            }
            // Force compiler to materialize the result
            asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "r"(len) : "memory");
          }
          else if constexpr(RET == RETURN_TYPE::STD_STRING)
          {
            std::string bin2chars;
            if constexpr(FMT == FORMAT::EXPONENTIAL)
            {
              bin2chars = Bin2Chars::Numeric::Floating::ExponentialNotation::ToStr(current_num, PRECISION);
            }
            else if constexpr(FMT == FORMAT::DECIMAL)
            {
              bin2chars = Bin2Chars::Numeric::Floating::DigitsPrecision::ToStr(current_num, PRECISION);
            }
            // Force compiler to materialize the result
            asm volatile("" : : "r"(bin2chars.data()) : "memory");
          }
        }

        const auto end = timer.stop();

        const auto delta = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin, end);

        simdy_tsc[i] = delta.tsc_ticks;
        simdy_clk[i] = delta.clk_cycles;
        simdy_ref[i] = delta.ref_cycles;
      }

      // std-lib
      {
        if constexpr(RET == RETURN_TYPE::BUFFERED)
        {
          Bin2Chars::Helpers::Assembly::prefetch_elements<2048>(&buff[0]);
        }

        Bin2Chars::Helpers::Assembly::prefetch_elements<BATCH>(&random_inputs[i]);

        const auto begin = timer.start();

        for(size_t j = 0, idx = i * BATCH; j < BATCH; j++, idx++)
        {
          const auto current_num = random_inputs[idx];
          if constexpr(RET == RETURN_TYPE::BUFFERED)
          {
            std::to_chars_result res;
            if constexpr(FMT == FORMAT::EXPONENTIAL)
            {
              res = std::to_chars(&buff[0], &buff[2048], current_num, std::chars_format::scientific, PRECISION);
            }
            else if constexpr(FMT == FORMAT::DECIMAL)
            {
              res = std::to_chars(&buff[0], &buff[2048], current_num, std::chars_format::fixed, PRECISION);
            }
            // Force compiler to materialize the result
            asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "m"(res) : "memory");
          }
          else if constexpr(RET == RETURN_TYPE::STD_STRING)
          {
            std::string stdlib;
            if constexpr(FMT == FORMAT::EXPONENTIAL)
            {
              stdlib = Bin2Chars::Numeric::Std::to_string<true>(current_num, PRECISION);
            }
            else if constexpr(FMT == FORMAT::DECIMAL)
            {
              stdlib = Bin2Chars::Numeric::Std::to_string<false>(current_num, PRECISION);
            }
            // Force compiler to materialize the result
            asm volatile("" : : "r"(stdlib.data()) : "memory");
          }
        }

        const auto end = timer.stop();

        const auto delta = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin, end);

        std_tsc[i] = delta.tsc_ticks;
        std_clk[i] = delta.clk_cycles;
        std_ref[i] = delta.ref_cycles;
      }

      // ryu
      {
        if constexpr(RET == RETURN_TYPE::BUFFERED)
        {
          Bin2Chars::Helpers::Assembly::prefetch_elements<2048>(&buff[0]);
        }

        Bin2Chars::Helpers::Assembly::prefetch_elements<BATCH>(&random_inputs[i]);

        const auto begin = timer.start();

        for(size_t j = 0, idx = i * BATCH; j < BATCH; j++, idx++)
        {
          const auto current_num = random_inputs[idx];
          if constexpr(RET == RETURN_TYPE::BUFFERED)
          {
            int res;
            if constexpr(FMT == FORMAT::EXPONENTIAL)
            {
              res = d2exp_buffered_n(static_cast<double>(current_num), static_cast<unsigned>(PRECISION), &buff[0]);
            }
            else if constexpr(FMT == FORMAT::DECIMAL)
            {
              res = d2fixed_buffered_n(static_cast<double>(current_num), static_cast<unsigned>(PRECISION), &buff[0]);
            }
            // Force compiler to materialize the result
            asm volatile("" : : "m"(*reinterpret_cast<char (*)[64]>(buff)), "m"(res) : "memory");
          }
          else if constexpr(RET == RETURN_TYPE::STD_STRING)
          {
            std::string stdlib;
            if constexpr(FMT == FORMAT::EXPONENTIAL)
            {
              stdlib = Bin2Chars::Numeric::Ryu::Exponential::ToStr(current_num, PRECISION);
            }
            else if constexpr(FMT == FORMAT::DECIMAL)
            {
              stdlib = Bin2Chars::Numeric::Ryu::Fixed::ToStr(current_num, PRECISION);
            }
            // Force compiler to materialize the result
            asm volatile("" : : "r"(stdlib.data()) : "memory");
          }
        }

        const auto end = timer.stop();

        const auto delta = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin, end);

        ryu_tsc[i] = delta.tsc_ticks;
        ryu_clk[i] = delta.clk_cycles;
        ryu_ref[i] = delta.ref_cycles;
      }

      {
        const auto begin = timer.start();

        for(size_t j = 0; j < BATCH; ++j)
        {
          asm volatile("" ::: "memory");
        }

        const auto end = timer.stop();

        const auto delta = Bin2Chars::Benchmark::Measurements::PmuTimer::delta(begin, end);

        empty_tsc[i] = delta.tsc_ticks;
        empty_clk[i] = delta.clk_cycles;
        empty_ref[i] = delta.ref_cycles;
      }

      std::this_thread::yield(); // so timer interrups 'can' (big enphasis on CAN) happen here instead of in the middle of measurements (hopefully)
    }

    Bin2Chars::Benchmark::PrintPmuResults(
        test_t{}, json, std::string(to_string(RET)) + " " + to_string(FMT) + " ftoa", PRECISION, BATCH,
        Bin2Chars::Benchmark::PmuResult{
            .label = "BIN2CHARS", .tsc = simdy_tsc, .core = simdy_clk, .ref = simdy_ref, .empty_tsc = empty_tsc, .empty_core = empty_clk, .empty_ref = empty_ref },
        Bin2Chars::Benchmark::PmuResult{
            .label = "STD_LIB", .tsc = std_tsc, .core = std_clk, .ref = std_ref, .empty_tsc = empty_tsc, .empty_core = empty_clk, .empty_ref = empty_ref },
        Bin2Chars::Benchmark::PmuResult{
            .label = "RYU", .tsc = ryu_tsc, .core = ryu_clk, .ref = ryu_ref, .empty_tsc = empty_tsc, .empty_core = empty_clk, .empty_ref = empty_ref });
  }
} // namespace

int main(int /*unused*/, char ** /*unused*/)
{

  try
  {
    constexpr auto TRIALS = 25'000; // 100'000
    constexpr auto BATCHES = 1'000;

    // pin to a core to avoid cross-core TSC sync issues
    Bin2Chars::Benchmark::SystemInfo::cpu_id = 0;
    Bin2Chars::Helpers::Assembly::pin_thread_to_cpu(Bin2Chars::Benchmark::SystemInfo::cpu_id);

    Bin2Chars::Benchmark::Store::File file{};
    CJParse::CJParse JSON{ "null" };

    const auto cpu_freqs = Bin2Chars::Benchmark::SystemInfo::get_cpu_freqs();
    const auto cpu_infos = Bin2Chars::Benchmark::SystemInfo::get_cpu_info();
    CJParse::Types::Object cpu_stuff{};

    cpu_stuff["model name"] = cpu_infos.model_name;
    cpu_stuff["microcode"] = cpu_infos.microcode;
    cpu_stuff["cache size"] = cpu_infos.cache_size;
    cpu_stuff["cache alignment"] = cpu_infos.cache_aligment;
    cpu_stuff["minimum frequency"] = cpu_freqs.min;
    cpu_stuff["maximum frequency"] = cpu_freqs.max;
    cpu_stuff["average frequency"] = cpu_freqs.avg;

    JSON.JSON = CJParse::CJParse::JsonValue{};
    auto &json = JSON.JSON;

    json["cpu info"] = cpu_stuff;
    json["trials"] = TRIALS;
    json["batch size"] = BATCHES;

    /*
    // exponenetial buffered
    {
      constexpr auto FMT = FORMAT::EXPONENTIAL;
      constexpr auto STORAGE = RETURN_TYPE::BUFFERED;

      json["format"] = (FMT == FORMAT::EXPONENTIAL) ? "exponential" : "decimal";
      json["type"] = (STORAGE == RETURN_TYPE::BUFFERED) ? "buffered" : "std_string";
      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 20, 50, 100 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, float>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), float>(JSON, FMT == FORMAT::EXPONENTIAL);

      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 15, 16, 17, 18, 20, 50, 100, 150, 200, 250, 300, 400, 500 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, double>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), double>(JSON, FMT == FORMAT::EXPONENTIAL);
    }
    */

    // digits buffered
    {
      constexpr auto FMT = FORMAT::DECIMAL;
      constexpr auto STORAGE = RETURN_TYPE::BUFFERED;

      json["format"] = (FMT == FORMAT::EXPONENTIAL) ? "exponential" : "decimal";
      json["type"] = (STORAGE == RETURN_TYPE::BUFFERED) ? "buffered" : "std_string";
      json["yields"] = CJParse::Types::Array{};

      /*
      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 20, 50, 100 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, float>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }
      */

      // file.Store<to_string(STORAGE), float>(JSON, FMT == FORMAT::EXPONENTIAL);

      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 15, 16, 17, 18, 20, 50, 100, 150, 200, 250, 300, 400, 500 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, double>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), double>(JSON, FMT == FORMAT::EXPONENTIAL);
    }

    // exponenetial std::string
    {
      constexpr auto FMT = FORMAT::EXPONENTIAL;
      constexpr auto STORAGE = RETURN_TYPE::STD_STRING;

      json["format"] = (FMT == FORMAT::EXPONENTIAL) ? "exponential" : "decimal";
      json["type"] = (STORAGE == RETURN_TYPE::BUFFERED) ? "buffered" : "std_string";
      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 20, 50, 100 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, float>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), float>(JSON, FMT == FORMAT::EXPONENTIAL);

      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 15, 16, 17, 18, 20, 50, 100, 150, 200, 250, 300, 400, 500 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, double>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), double>(JSON, FMT == FORMAT::EXPONENTIAL);
    }

    // digits std::string
    {
      constexpr auto FMT = FORMAT::DECIMAL;
      constexpr auto STORAGE = RETURN_TYPE::STD_STRING;

      json["format"] = (FMT == FORMAT::EXPONENTIAL) ? "exponential" : "decimal";
      json["type"] = (STORAGE == RETURN_TYPE::BUFFERED) ? "buffered" : "std_string";
      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 20, 50, 100 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, float>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), float>(JSON, FMT == FORMAT::EXPONENTIAL);

      json["yields"] = CJParse::Types::Array{};

      for(const auto &precision : { 0, 1, 2, 5, 8, 10, 15, 16, 17, 18, 20, 50, 100, 150, 200, 250, 300, 400, 500 })
      {
        CJParse::Types::Object this_pres;

        TestFloatingType<TRIALS, BATCHES, FMT, STORAGE, double>(this_pres["precision " + std::to_string(precision)], precision);

        json["yields"].as_array().emplace_back(this_pres);
      }

      // file.Store<to_string(STORAGE), double>(JSON, FMT == FORMAT::EXPONENTIAL);
    }
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
