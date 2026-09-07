#pragma once
#include <boost/type_index.hpp>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace Bin2Chars::Tests
{
  struct LogHexStr
  {
    std::string_view label;
    std::string_view num_str;
    LogHexStr(const std::string &_label, const std::string &_num_str) : label(_label), num_str(_num_str) {};
  };

  template <typename... Args>
  void log_str_and_into_hex(const Args &...logs)
  {
    std::string log = "we have:";

    ((log += std::format(" {} = '{}'", logs.label, logs.num_str)), ...);

    std::cout << log << std::endl;

    auto print_hex = [](std::string_view in)
    {
      for(const char c : in)
      {
        std::cout << std::hex << static_cast<int>(c) << " ";
      }

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
  void log_time_tables(T /*unused*/, const char *ACTION, const int &PRECISION, const Args &...times)
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
      {
        return RESET;
      }

      double ratio = static_cast<double>(val.count()) / average_ns;

      if(std::abs(1.0 - ratio) <= 0.03)
      {
        return YELLOW;
      }

      return (static_cast<double>(val.count()) < average_ns) ? GREEN : RED;
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

    const std::string type_name = boost::typeindex::type_id<T>().pretty_name();
    std::string title = std::format("Action '{}' with precision '{}' {} COMPARISON (Avg: {:.3f} millisec) ", ACTION, PRECISION, type_name, average_ns / 1'000'000);
    int total_width = 15 + (SIZE * 18); // 15 for label + 18 per column (| + color + 15 chars)

    std::cout << "\n" << std::format("{:=^{}}", title, total_width) << "\n";
    std::cout << header_row << "\n";
    std::cout << std::string(static_cast<size_t>(total_width), '-') << "\n";
    std::cout << row_sec << "\n";
    std::cout << row_milli << "\n";
    std::cout << row_micro << "\n";
    std::cout << row_cpu_cycles << "\n";
    std::cout << std::string(static_cast<size_t>(total_width), '=') << "\n";
  }

} // namespace Bin2Chars::Tests
