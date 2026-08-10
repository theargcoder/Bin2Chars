#define BOOST_TEST_MODULE IntegerTests
#include <boost/test/included/unit_test.hpp>
#include <boost/type_index.hpp>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

#include "include/Algos/Competition.h"
#include "include/Algos/Integer.h"
#include "include/Helpers/Math.h"

namespace
{
  struct LogHexStr
  {
    std::string_view label;
    std::string_view num_str;
    LogHexStr(const std::string &_label, const std::string &_num_str) : label(_label), num_str(_num_str) {};
  };

  template <typename... Args>
  auto log_str_and_into_hex(const Args &...logs)
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
  auto log_time_tables(T, const char *ACTION, const int &N, const Args &...times)
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

    std::string type_name = boost::typeindex::type_id<T>().pretty_name();
    std::string title = std::format("Action '{}' digits '{}' {} COMPARISON (Avg: {:.3f} millisec) ", ACTION, N, type_name, average_ns / 1'000'000);
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
  template <uint64_t N, typename Type>
  auto looper_ints(const bool &PLUS, const Type &DELIM, const Type &JUMP, auto &open_logging_time, auto &open_logging_cpu_cycles, auto &std_lib_time, auto &std_lib_cpu_cycles,
                   auto &std_lib_to_str_time, auto &std_lib_to_str_cycles, auto &simdy_time, auto &simdy_cycles) -> void
  {
    const constexpr auto WISHED_RANGE = 1'000'000;
    const constexpr auto MAX_NUM = std::numeric_limits<Type>::max();
    const constexpr Type RANGE = WISHED_RANGE < MAX_NUM ? static_cast<Type>(WISHED_RANGE) : MAX_NUM;
    const constexpr Type MAX_ERRORS = 10;

    uint32_t errors = 0;
    uint64_t cycles = 0;
    // OpenLogging logger;

    std::string our_log, std_log, std_lib_to_str_log, simdy_log;
    while(cycles < WISHED_RANGE && errors < MAX_ERRORS)
    {
      cycles += RANGE;
      for(Type i = DELIM, lim = 0, max_iter = 0; ((PLUS) ? i < DELIM + RANGE : i > DELIM - RANGE) && lim < MAX_ERRORS && max_iter < RANGE;
          (PLUS) ? i += JUMP : i -= JUMP, max_iter++)
      {
        const auto st_log = Helpers::Assembly::timer_start();
        our_log = Helpers::Numeric::Integral::ToStr(i);
        const auto en_log = Helpers::Assembly::timer_end();

        const auto st_std_to_str = Helpers::Assembly::timer_start();
        std_log = Helpers::Numeric::Std::to_string<false>(i, 123);
        const auto en_std_to_str = Helpers::Assembly::timer_end();

        const auto std_lib_to_st = Helpers::Assembly::timer_start();
        std_lib_to_str_log = std::to_string(i);
        const auto std_lib_to_en = Helpers::Assembly::timer_end();

        const auto simdy_st = Helpers::Assembly::timer_start();
        simdy_log = Helpers::Numeric::Integral::ToStrSIMD(i);
        const auto simdy_en = Helpers::Assembly::timer_end();

        open_logging_time += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_log - st_log)));
        open_logging_cpu_cycles += en_log - st_log;
        std_lib_time += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(en_std_to_str - st_std_to_str)));
        std_lib_cpu_cycles += en_std_to_str - st_std_to_str;
        std_lib_to_str_time
            += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(std_lib_to_en - std_lib_to_st)));
        std_lib_to_str_cycles += std_lib_to_en - std_lib_to_st;
        simdy_time += std::chrono::duration_cast<std::chrono::nanoseconds>(static_cast<std::chrono::nanoseconds>(Helpers::Assembly::rdtsc_to_ns(simdy_en - simdy_st)));
        simdy_cycles += simdy_en - simdy_st;

        if(our_log != std_log || simdy_log != std_lib_to_str_log)
        {
          BOOST_CHECK_EQUAL(our_log, std_log);
          log_str_and_into_hex(LogHexStr("Helpers::Numeric::ToStr", our_log), LogHexStr("std::to_chars", std_log), LogHexStr("std::to_string", std_lib_to_str_log),
                               LogHexStr("Helpers::Numeric::ToStrSIMD", simdy_log));

          lim++;
          errors++;
        }
      }
    }
  };

  template <uint64_t N, typename T>
  auto tester_ints(const T &) -> auto
  {
    std::chrono::nanoseconds tostr_integral_ours_took{ 0 };
    std::chrono::nanoseconds std_to_chars_took{ 0 };
    std::chrono::nanoseconds std_lib_to_str_time{ 0 };
    std::chrono::nanoseconds simdy_lib_time{ 0 };
    uint64_t helpers_math_cpu_cycles{ 0 };
    uint64_t std_lib_cpu_cycles{ 0 };
    uint64_t std_lib_to_str_cycles{ 0 };
    uint64_t simdy_lib_cycles{ 0 };

    const constexpr auto MIN = std::numeric_limits<T>::min();
    const constexpr auto MAX = std::numeric_limits<T>::max();
    const constexpr T UNIT = T{ 1 };

    // ---- Extremes and Zero Region ----
    looper_ints<N>(true, MIN, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);
    looper_ints<N>(false, MAX, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);
    ;
    looper_ints<N>(true, T{ 0 }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);
    looper_ints<N>(false, T{ 0 }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                   simdy_lib_time, simdy_lib_cycles);

    // ---- Around powers of two (Bit boundaries) ----
    for(int e = 1; e < std::numeric_limits<T>::digits; ++e)
    {
      const T val = UNIT << e;
      looper_ints<N>(true, val, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
      looper_ints<N>(false, val, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
    }

    // ---- Around powers of ten (String length boundaries) ----
    for(T val = 10; val > 0 && val < MAX / 10; val *= 10)
    {
      looper_ints<N>(true, val, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
      looper_ints<N>(false, val, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
    }

    // ---- Large magnitude sweeps (Sparse) ----
    if constexpr(sizeof(T) >= 4)
    {
      looper_ints<N>(true, MIN / 2, T{ 123 }, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
      looper_ints<N>(false, MAX / 2, T{ 123 }, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles,
                     simdy_lib_time, simdy_lib_cycles);
    }

    // ---- Randomish coverage ----
    looper_ints<N>(true, T{ MAX / T{ 10 } }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time,
                   std_lib_to_str_cycles, simdy_lib_time, simdy_lib_cycles);
    looper_ints<N>(false, T{ MAX / T{ 10 } * T{ 9 } }, UNIT, tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time,
                   std_lib_to_str_cycles, simdy_lib_time, simdy_lib_cycles);

    return std::make_tuple(tostr_integral_ours_took, helpers_math_cpu_cycles, std_to_chars_took, std_lib_cpu_cycles, std_lib_to_str_time, std_lib_to_str_cycles, simdy_lib_time,
                           simdy_lib_cycles);
  }

  template <typename T, size_t... I>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_int_impl(std::index_sequence<I...>)
  {
    auto res = tester_ints<1>(T{ 0 });
    ((res = tester_ints<I + 1>(T{ 0 }),
      log_time_tables(T{ 0 }, "INTEGERS", I + 1, BenchResult("Numeric:ToStr", std::get<0>(res), std::get<1>(res)), BenchResult("std::to_chars", std::get<2>(res), std::get<3>(res)),
                      BenchResult("std::to_string", std::get<4>(res), std::get<5>(res)), BenchResult("Numeric::ToStrSIMD", std::get<6>(res), std::get<7>(res)))),
     ...);
  };

  template <typename T>
    requires std::is_integral_v<T>
  const auto test_and_benchmark_ints(T)
  {
    test_and_benchmark_int_impl<T>(std::make_index_sequence<2>{});
  };

} // namespace

BOOST_AUTO_TEST_CASE(test_all_integegral_v)
{
  Helpers::Assembly::pin_thread_to_cpu(3);

  // test_and_benchmark_ints<int8_t>(0);
  // test_and_benchmark_ints<uint8_t>(0);
  // test_and_benchmark_ints<int16_t>(0);
  // test_and_benchmark_ints<uint16_t>(0);
  // test_and_benchmark_ints<int32_t>(0);
  // test_and_benchmark_ints<uint32_t>(0);
  test_and_benchmark_ints<int64_t>(0);
  test_and_benchmark_ints<uint64_t>(0);
}
