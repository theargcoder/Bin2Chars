#define BOOST_TEST_MODULE ScientificNotatioTests
#include <boost/test/included/unit_test.hpp>

#include <bit>
#include <chrono>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>

#include "include/Algos/Competition.h"
#include "include/Algos/Floating/DigitsPrecision.h"
#include "include/Algos/Floating/Exponential.h"
#include "include/Helpers/Math.h"

namespace
{
  struct LogHexStr
  {
    std::string_view label;
    std::string_view num_str;
    LogHexStr(const std::string &_label, const std::string &_num_str) : label(_label), num_str(_num_str) {};
  };

  const auto log_str_and_into_hex = []<typename... Args>(const Args &...logs)
  {
    std::string log = "we have:";

    ((log += std::format(" {} = '{}'", logs.label, logs.num_str)), ...);

    std::cout << log << std::endl;

    auto print_hex = [](std::string_view in)
    {
      for(unsigned char c : in)
        std::cout << std::hex << (int)c << " ";

      std::cout << std::dec << '\n';
    };

    (print_hex(logs.num_str), ...);
  };

  struct BenchResult
  {
    std::string_view label;
    std::chrono::nanoseconds time;
    uint64_t cycles;

    BenchResult(const char *str, std::chrono::nanoseconds nano, uint64_t cpu_cycles) : label(str), time(nano), cycles(cpu_cycles) {};
  };

  template <typename T, typename... Args>
  auto log_time_tables(T, const char *ACTION, const int &PRECISION, const Args &...times)
  {
    using namespace std::chrono;

    // ANSI Color Codes
    const std::string_view RESET = "\033[0m";
    const std::string_view GREEN = "\033[32m";
    const std::string_view RED = "\033[31m";
    const std::string_view YELLOW = "\033[33m";

    const auto SIZE = sizeof...(times);

    // Calculate average time (using double to keep precision)
    const auto total_cpu_cycles = (times.cycles + ...);
    const double average_cycles = static_cast<double>(total_cpu_cycles) / SIZE;
    const auto total_ns = (times.time + ...).count();
    const double average_ns = static_cast<double>(total_ns) / SIZE;

    auto get_color = [&](nanoseconds val) -> std::string_view
    {
      if(val.count() == 0)
        return RESET;

      double ratio = static_cast<double>(val.count()) / average_ns;

      if(std::abs(1.0 - ratio) <= 0.03)
        return YELLOW;

      return (val.count() < average_ns) ? GREEN : RED;
    };

    const auto get_label_cell = [&](const BenchResult &res) { return std::format(" | {: >15}", res.label); };

    const auto get_val_cell = [&](const BenchResult &res, auto unit_type)
    {
      auto color = get_color(res.time);
      auto val = duration_cast<duration<double, typename decltype(unit_type)::period>>(res.time).count();
      return std::format(" | {}{: >15.3f}{}", color, val, RESET);
    };

    auto get_val_cpu_cycles = [&](const BenchResult &res)
    {
      auto color = (static_cast<double>(res.cycles) < average_cycles) ? GREEN : RED;
      return std::format(" | {}{: >15}{}", color, res.cycles, RESET);
    };

    std::string header_row = std::format("{:>15}", "Unit");
    ((header_row += get_label_cell(times)), ...);

    std::string row_sec = std::format("{:>15}", "Seconds");
    ((row_sec += get_val_cell(times, seconds{})), ...);

    std::string row_milli = std::format("{:>15}", "Milliseconds");
    ((row_milli += get_val_cell(times, milliseconds{})), ...);

    std::string row_micro = std::format("{:>15}", "Microseconds");
    ((row_micro += get_val_cell(times, microseconds{})), ...);

    std::string row_cpu_cycles = std::format("{:>15}", "Cpu Cycles");
    ((row_cpu_cycles += get_val_cpu_cycles(times)), ...);

    std::string type_name = std::is_same_v<float, T> ? "float32_t" : "float64_t";
    std::string title = std::format("Action '{}' with precision '{}' {} COMPARISON (Avg: {:.3f} millisec) ", ACTION, PRECISION, type_name, average_ns / 1'000'000);
    int total_width = 15 + (SIZE * 18); // 15 for label + 18 per column (| + color + 15 chars)

    std::cout << "\n" << std::format("{:=^{}}", title, total_width) << "\n";
    std::cout << header_row << "\n";
    std::cout << std::string(total_width, '-') << "\n";
    std::cout << row_sec << "\n";
    std::cout << row_milli << "\n";
    std::cout << row_micro << "\n";
    std::cout << row_cpu_cycles << "\n";
    std::cout << std::string(total_width, '=') << "\n";
  }

} // namespace

namespace
{
  const auto fuzzer_format_exponential = []<typename Type>(const Type &, const int &PRECISION, const size_t SAMPLES, auto &open_logging_took, auto &open_logging_cycles,
                                                           auto &std_fmt_took, auto &std_cycles, auto &ryu_took, auto &ryu_cycles) -> void
  {
    using UIntType = std::conditional_t<sizeof(Type) == 4, uint32_t, uint64_t>;

    // Fixed seed so test failures are 100% reproducible
    std::mt19937_64 rng(0xDEADBEEF);
    std::uniform_int_distribution<UIntType> dist(0, std::numeric_limits<UIntType>::max());

    size_t errors = 0;
    const constexpr size_t MAX_ERRORS = 10;

    for(size_t i = 0; i < SAMPLES && errors < MAX_ERRORS; ++i)
    {
      UIntType raw_bits = dist(rng);
      Type val = std::bit_cast<Type>(raw_bits);

      std::string open_logging, std_format, ryu;

      const auto st_open_logging = Helpers::Assembly::timer_start();
      open_logging = Helpers::Numeric::Floating::DigitsPrecision::ToStr<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);
      const auto en_open_logging = Helpers::Assembly::timer_end();

      const auto st_std_fmt = Helpers::Assembly::timer_start();
      std_format = Helpers::Numeric::Std::to_string<false>(val, PRECISION);
      const auto en_std_fmt = Helpers::Assembly::timer_end();

      const auto st_ryu = Helpers::Assembly::timer_start();
      ryu = Helpers::Numeric::Ryu::Fixed::ToStr(val, PRECISION);
      const auto en_ryu = Helpers::Assembly::timer_end();

      open_logging_took
          += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_open_logging - st_open_logging)));
      open_logging_cycles += en_open_logging - st_open_logging;
      std_fmt_took += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_std_fmt - st_std_fmt)));
      std_cycles += en_std_fmt - st_std_fmt;
      ryu_took += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_ryu - st_ryu)));
      ryu_cycles += en_ryu - st_ryu;

      if(open_logging != std_format)
      {
        if(open_logging.contains("nan") && std_format.contains("nan"))
        {
          continue;
        }

        const auto log_val = std::strtold(open_logging.c_str(), nullptr);
        const auto ref_val = std::strtold(std_format.c_str(), nullptr);

        if(log_val != ref_val)
        {
          BOOST_CHECK_EQUAL(log_val, ref_val);
          log_str_and_into_hex(LogHexStr("open_logging", open_logging), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

          open_logging = Helpers::Numeric::Floating::DigitsPrecision::ToStr<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);

          char buffer[1024];
          d2exp_buffered(val, PRECISION, buffer);

          errors++;
        }
      }
    }
  };

  const auto lopper_format_exponential = []<typename Type>(const int &PRECISION, const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging_took,
                                                           auto &open_logging_cycles, auto &std_fmt_took, auto &std_cycles, auto &ryu_took, auto &ryu_cycles) -> void
  {
    const constexpr auto WISHED_RANGE = 100'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? WISHED_RANGE : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    // OpenLogging logger;

    for(Type val = DELIM, lim = 0, max_iter = 0; ((PLUS) ? val < DELIM + RANGE : val > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE;
        (PLUS) ? val += JUMP : val -= JUMP, max_iter++)
    {
      std::string open_logging, std_format, ryu;

      const auto st_open_logging = Helpers::Assembly::timer_start();
      open_logging = Helpers::Numeric::Floating::DigitsPrecision::ToStr<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);
      const auto en_open_logging = Helpers::Assembly::timer_end();

      const auto st_std_fmt = Helpers::Assembly::timer_start();
      std_format = Helpers::Numeric::Std::to_string<false>(val, PRECISION);
      const auto en_std_fmt = Helpers::Assembly::timer_end();

      const auto st_ryu = Helpers::Assembly::timer_start();
      ryu = Helpers::Numeric::Ryu::Fixed::ToStr(val, PRECISION);
      const auto en_ryu = Helpers::Assembly::timer_end();

      open_logging_took
          += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_open_logging - st_open_logging)));
      open_logging_cycles += en_open_logging - st_open_logging;
      std_fmt_took += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_std_fmt - st_std_fmt)));
      std_cycles += en_std_fmt - st_std_fmt;
      ryu_took += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_ryu - st_ryu)));
      ryu_cycles += en_ryu - st_ryu;

      if(open_logging != std_format)
      {
        const auto log_val = std::strtold(open_logging.c_str(), nullptr);
        const auto ref_val = std::strtold(std_format.c_str(), nullptr);

        if(open_logging.contains("nan") && std_format.contains("nan"))
          continue;

        if(log_val != ref_val) // if(!almost_equal(i, log_val, ref_val))
        {
          BOOST_CHECK_EQUAL(log_val, ref_val);
          log_str_and_into_hex(LogHexStr("open_logging", open_logging), LogHexStr("std::format", std_format), LogHexStr("ryu", ryu));

          open_logging = Helpers::Numeric::Floating::DigitsPrecision::ToStr<Helpers::Numeric::Floating::DigitsPrecision::RoundingBehavior::ROUND>(val, PRECISION);

          char buffer[1024];
          d2exp_buffered(val, PRECISION, buffer);

          lim++;
        }
      }
    }
  };

  const auto tester_format_exponential = []<typename T>(const T &bannana, const int &PRECISION)
  {
    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto DENORM = std::numeric_limits<T>::denorm_min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr auto EPS = std::numeric_limits<T>::epsilon();

    std::chrono::nanoseconds open_logging_time{ 0 };
    uint64_t open_logging_cycles{ 0 };
    std::chrono::nanoseconds std_fmt_time{ 0 };
    uint64_t std_fmt_cycles{ 0 };
    std::chrono::nanoseconds ryu_time{ 0 };
    uint64_t ryu_cycles{ 0 };

    // ---- small / subnormal region ----
    lopper_format_exponential(PRECISION, true, T{ 0 }, DENORM, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, MIN, DENORM, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- small normal numbers ----
    lopper_format_exponential(PRECISION, true, MIN, EPS, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, MIN * T{ 10 }, EPS, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- around powers of two ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::ldexp(T{ 1 }, e); // 2^e
      lopper_format_exponential(PRECISION, true, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
      lopper_format_exponential(PRECISION, false, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    }

    // ---- around powers of ten ----
    for(int e = -20; e <= 20; ++e)
    {
      const T val = std::pow(T{ 10 }, e);
      lopper_format_exponential(PRECISION, true, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
      lopper_format_exponential(PRECISION, false, val, EPS * val, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    }

    // ---- medium magnitude sweeps ----
    lopper_format_exponential(PRECISION, true, T{ 1 }, EPS, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, T{ 100 }, EPS * T{ 100 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, T{ 1e6 }, EPS * T{ 1e6 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- large numbers ----
    lopper_format_exponential(PRECISION, false, MAX, EPS * MAX, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, false, MAX / T{ 10 }, EPS * MAX, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, false, MAX / T{ 1000 }, EPS * MAX, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, T{ 1.234 }, T{ 0.0001 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, T{ 123.456 }, T{ 0.01 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, false, T{ 98765.4321 }, T{ 0.1 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- randomish mantissa coverage ----
    lopper_format_exponential(PRECISION, true, T{ 1.234 }, T{ 0.0001 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, true, T{ 123.456 }, T{ 0.01 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
    lopper_format_exponential(PRECISION, false, T{ 98765.4321 }, T{ 0.1 }, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    // ---- MASSIVE CHAOS FUZZER ----
    // 1 million purely random bit-patterns per precision level
    fuzzer_format_exponential(bannana, PRECISION, 1'000'000, open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);

    return std::make_tuple(open_logging_time, open_logging_cycles, std_fmt_time, std_fmt_cycles, ryu_time, ryu_cycles);
  };

  const auto test_and_benchmark_float = []<typename T>
    requires std::is_floating_point_v<T>
  (const T &, const int &PRECISION)
  {
    const auto float_res = tester_format_exponential(static_cast<T>(0), PRECISION);

    log_time_tables(T{ 0.0 }, "Exponential Formatting ", PRECISION, BenchResult("OpenLogging", std::get<0>(float_res), std::get<1>(float_res)),
                    BenchResult("std::to_chars", std::get<2>(float_res), std::get<3>(float_res)), BenchResult("ryu", std::get<4>(float_res), std::get<5>(float_res)));
  };

} // namespace

BOOST_AUTO_TEST_CASE(test_all_floating_point_v)
{
  // floats
  test_and_benchmark_float(static_cast<float>(0), 1);
  test_and_benchmark_float(static_cast<float>(0), 2);
  test_and_benchmark_float(static_cast<float>(0), 3);
  test_and_benchmark_float(static_cast<float>(0), 4);
  test_and_benchmark_float(static_cast<float>(0), 5);
  test_and_benchmark_float(static_cast<float>(0), 6);
  // doubles
  // test_and_benchmark_float(static_cast<double>(0), 1);
  // test_and_benchmark_float(static_cast<double>(0), 2);
  // test_and_benchmark_float(static_cast<double>(0), 3);
  // test_and_benchmark_float(static_cast<double>(0), 4);
  // test_and_benchmark_float(static_cast<double>(0), 5);
  // test_and_benchmark_float(static_cast<double>(0), 6);
  // test_and_benchmark_float(static_cast<double>(0), 7);
  // test_and_benchmark_float(static_cast<double>(0), 8);
  // test_and_benchmark_float(static_cast<double>(0), 9);
  // test_and_benchmark_float(static_cast<double>(0), 10);
  // test_and_benchmark_float(static_cast<double>(0), 11);
  // test_and_benchmark_float(static_cast<double>(0), 12);
  // test_and_benchmark_float(static_cast<double>(0), 13);
  // test_and_benchmark_float(static_cast<double>(0), 14);
  // test_and_benchmark_float(static_cast<double>(0), 15);
  // test_and_benchmark_float(static_cast<double>(0), 16);
  // test_and_benchmark_float(static_cast<double>(0), 17);

  // tester_format_exponential(static_cast<long double>(0));
}
///
