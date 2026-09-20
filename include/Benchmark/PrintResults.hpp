#pragma once

#include "include/Benchmark/StoreResults.hpp"
#include "include/Benchmark/SystemInfo.hpp"
#include <boost/type_index.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <CJParse/include/CJParse.hpp>

namespace Bin2Chars::Benchmark
{
  struct LogHexStr
  {
    std::string_view label;
    std::string_view num_str;

    LogHexStr(const std::string &_label, const std::string &_num_str) : label(_label), num_str(_num_str)
    {
    }
  };

  struct BenchResult
  {
    std::string_view label;
    std::chrono::nanoseconds time;
    std::uint64_t cycles;

    BenchResult(const char *str, const std::chrono::nanoseconds nano, const std::uint64_t cpu_cycles) : label(str), time(nano), cycles(cpu_cycles)
    {
    }
  };

  // --------------------------------------------------------------------------
  // PMU benchmark data
  // --------------------------------------------------------------------------

  struct PmuResult
  {
    std::string_view label;

    const std::vector<std::uint64_t> &tsc;
    const std::vector<std::uint64_t> &core;
    const std::vector<std::uint64_t> &ref;

    const std::vector<std::uint64_t> &empty_tsc;
    const std::vector<std::uint64_t> &empty_core;
    const std::vector<std::uint64_t> &empty_ref;
  };

  struct PmuMetricStats
  {
    double min;
    double median;
    double mean;
    double p95;
    double p99;
  };

  struct PmuResultStats
  {
    std::string_view label;

    std::size_t trials;

    PmuMetricStats tsc;
    PmuMetricStats core;
    PmuMetricStats ref;
  };

  inline PmuMetricStats MakePmuMetricStats(const std::vector<std::uint64_t> &samples, const std::vector<std::uint64_t> &empty, const std::size_t batch)
  {
    if(samples.size() != empty.size())
    {
      throw std::invalid_argument("PMU sample and EMPTY vectors must have identical sizes");
    }

    if(samples.empty())
    {
      throw std::invalid_argument("PMU sample vector cannot be empty");
    }

    if(batch == 0)
    {
      throw std::invalid_argument("PMU batch size cannot be zero");
    }

    std::vector<double> corrected(samples.size());

    const auto batch_d = static_cast<double>(batch);

    for(std::size_t i = 0; i < samples.size(); ++i)
    {
      const double sample = (static_cast<double>(samples[i]) - static_cast<double>(empty[i])) / batch_d;
      corrected[i] = std::max(0.0, sample);
    }

    std::ranges::sort(corrected);

    const double mean = std::accumulate(corrected.begin(), corrected.end(), 0.0) / static_cast<double>(corrected.size());

    const std::size_t median_index = corrected.size() / 2;
    const std::size_t p95_index = corrected.size() * 95 / 100;
    const std::size_t p99_index = corrected.size() * 99 / 100;

    return { .min = corrected.front(), .median = corrected[median_index], .mean = mean, .p95 = corrected[p95_index], .p99 = corrected[p99_index] };
  }

  template <typename T, typename... Results>
    requires(sizeof...(Results) > 0)
  void PrintPmuResults(T /*unused*/, CJParse::CJParse::JsonValue &JSON, const std::string &ACTION, const int &PRECISION, const std::size_t BATCH, const Results &...results)
  {
    const std::string_view RESET = "\033[0m";
    const std::string_view GREEN = "\033[32m";
    const std::string_view RED = "\033[31m";
    const std::string_view YELLOW = "\033[33m";

    std::vector<PmuResultStats> stats;
    stats.reserve(sizeof...(Results));

    (stats.push_back(PmuResultStats{ .label = results.label,
                                     .trials = results.tsc.size(),
                                     .tsc = MakePmuMetricStats(results.tsc, results.empty_tsc, BATCH),
                                     .core = MakePmuMetricStats(results.core, results.empty_core, BATCH),
                                     .ref = MakePmuMetricStats(results.ref, results.empty_ref, BATCH) }),
     ...);

    // All compared implementations must have the same number of trials.
    const std::size_t trials = stats.front().trials;

    for(const auto &result : stats)
    {
      if(result.trials != trials)
      {
        throw std::invalid_argument("All PMU results must have "
                                    "the same number of trials");
      }
    }

    const std::string type_name = Store::get_pretty_name<T>();

    // --------------------------------------------------------------------------
    // JSON
    // --------------------------------------------------------------------------

    const auto metric_to_json = [](const PmuMetricStats &metric) -> CJParse::Types::Object
    {
      CJParse::Types::Object json{};

      json["min"] = metric.min;
      json["median"] = metric.median;
      json["mean"] = metric.mean;
      json["p95"] = metric.p95;
      json["p99"] = metric.p99;

      return json;
    };

    for(const auto &result : stats)
    {
      auto &implementation = JSON[result.label];

      implementation["tsc"] = metric_to_json(result.tsc);
      implementation["core"] = metric_to_json(result.core);
      implementation["ref"] = metric_to_json(result.ref);
    }

    // --------------------------------------------------------------------------
    // Console output
    // --------------------------------------------------------------------------

    const auto winner_it = std::ranges::min_element(stats, {}, [](const PmuResultStats &result) { return result.core.median; });
    const auto winner_index = static_cast<std::size_t>(std::distance(stats.begin(), winner_it));
    const auto color_for = [&](const double value, const double best) -> std::string_view
    {
      if(std::abs(value - best) <= std::numeric_limits<double>::epsilon())
      {
        return GREEN;
      }

      if(std::abs(best - 0.0) <= std::numeric_limits<double>::epsilon() && std::abs(1.0 - (value / best)) <= 0.03)
      {
        return YELLOW;
      }

      return RED;
    };

    const auto append_header_cell = [&](std::string &row, const std::size_t index)
    {
      const bool winner = index == winner_index;
      const auto color = winner ? GREEN : RESET;
      const auto flag = winner ? "(W)" : "   ";

      row += std::format(" | {: >16} {}{}{}", stats[index].label, color, flag, RESET);
    };

    std::string header = std::format("{:>20}", "Statistic");

    for(std::size_t i = 0; i < stats.size(); ++i)
    {
      append_header_cell(header, i);
    }

    const auto print_metric_row = [&](const bool &SUB_TITLE, const std::string_view name, const auto getter)
    {
      std::string row = (SUB_TITLE) ? std::format("{:>20}", std::string(20, ' ')) : std::format("{:>20}", name);
      if(SUB_TITLE)
      {
        for(std::size_t i = 0; i < stats.size(); ++i)
        {
          row += std::format(" | {:_^20}", name);
        }
      }
      else
      {
        double best = getter(stats.front());

        for(std::size_t i = 1; i < stats.size(); ++i)
        {
          best = std::min(best, getter(stats[i]));
        }

        for(const auto &result : stats)
        {
          const double value = getter(result);
          const auto color = color_for(value, best);
          row += std::format(" | {}{: >20.3f}{}", color, value, RESET);
        }
      }

      std::cout << row << "\n";
    };

    // --------------------------------------------------------------------------
    // Actual output to stdout
    // --------------------------------------------------------------------------

    const int total_width = 20 + static_cast<int>(stats.size() * 25);

    const std::string title
        = std::format("Action '{}' with precision '{}' {} PMU COMPARISON ({} trials x {} calls, EMPTY-subtracted)", ACTION, PRECISION, type_name, trials, BATCH);

    std::cout << "\n" << std::format("{:=^{}}", title, total_width) << "\n";

    std::cout << header << "\n";

    std::cout << std::string(static_cast<std::size_t>(total_width), '-') << "\n";

    print_metric_row(true, "TSC", [](const PmuResultStats &r) { return r.tsc.min; });
    print_metric_row(false, "Min", [](const PmuResultStats &r) { return r.tsc.min; });
    print_metric_row(false, "Median", [](const PmuResultStats &r) { return r.tsc.median; });
    print_metric_row(false, "Mean", [](const PmuResultStats &r) { return r.tsc.mean; });
    print_metric_row(false, "P95", [](const PmuResultStats &r) { return r.tsc.p95; });
    print_metric_row(false, "P99", [](const PmuResultStats &r) { return r.tsc.p99; });

    std::cout << std::string(static_cast<std::size_t>(total_width), '-') << "\n";

    print_metric_row(true, "CORE", [](const PmuResultStats &r) { return r.tsc.min; });
    print_metric_row(false, "Min", [](const PmuResultStats &r) { return r.core.min; });
    print_metric_row(false, "Median", [](const PmuResultStats &r) { return r.core.median; });
    print_metric_row(false, "Mean", [](const PmuResultStats &r) { return r.core.mean; });
    print_metric_row(false, "P95", [](const PmuResultStats &r) { return r.core.p95; });
    print_metric_row(false, "P99", [](const PmuResultStats &r) { return r.core.p99; });

    std::cout << std::string(static_cast<std::size_t>(total_width), '-') << "\n";

    print_metric_row(true, "REF", [](const PmuResultStats &r) { return r.tsc.min; });
    print_metric_row(false, "Min", [](const PmuResultStats &r) { return r.ref.min; });
    print_metric_row(false, "Median", [](const PmuResultStats &r) { return r.ref.median; });
    print_metric_row(false, "Mean", [](const PmuResultStats &r) { return r.ref.mean; });
    print_metric_row(false, "P95", [](const PmuResultStats &r) { return r.ref.p95; });
    print_metric_row(false, "P99", [](const PmuResultStats &r) { return r.ref.p99; });

    std::cout << std::string(static_cast<std::size_t>(total_width), '=') << "\n";
  }

} // namespace Bin2Chars::Benchmark
