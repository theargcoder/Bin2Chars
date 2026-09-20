#pragma once

#include <filesystem>
#include <fstream>
#include <ios>
#include <string>

namespace Bin2Chars::Benchmark::SystemInfo
{
  extern const char avg[] = "avg";
  extern const char min[] = "min";
  extern const char max[] = "min";
  extern const char model_name[] = "model name";
  extern const char microcode[] = "microcode";
  extern const char cache_size[] = "cache size";
  extern const char cache_aligment[] = "cache_alignment";

  unsigned cpu_id = 0;

  struct cpu_freqs
  {
    unsigned avg, min, max;
  };

  struct cpu_info
  {
    std::string model_name, microcode, cache_size, cache_aligment;
  };

  template <const char *TYPE>
  unsigned get_cpu_freq_impl()
  {
    // /cpu0/cpufreq/cpuinfo_max_freq
    std::filesystem::path path("/sys/devices/system/cpu");
    path /= std::string("cpu") + std::to_string(cpu_id);
    path /= "cpufreq/cpuinfo_" + std::string(TYPE) + "_freq";
    unsigned freq = 0;

    std::fstream file;
    file.open(path, std::ios::in);

    if(file.is_open())
    {
      file >> freq;
    }

    return freq;
  }

  template <const char *TYPE>
  std::string get_cpu_info_impl()
  {
    std::filesystem::path path("/proc/cpuinfo");

    std::fstream file;
    file.open(path, std::ios::in);

    std::string to_return, line;
    if(file.is_open())
    {
      while(std::getline(file, line) && line.contains(TYPE) == false)
      {
      }
      const auto place = line.find(':');
      to_return = line.substr(place + 2);
    }

    return to_return;
  }

  inline cpu_freqs get_cpu_freqs()
  {
    cpu_freqs freqs{};
    freqs.avg = get_cpu_freq_impl<avg>();
    freqs.min = get_cpu_freq_impl<min>();
    freqs.max = get_cpu_freq_impl<max>();

    return freqs;
  }

  inline cpu_info get_cpu_info()
  {
    cpu_info info;
    info.model_name = get_cpu_info_impl<model_name>();
    info.microcode = get_cpu_info_impl<microcode>();
    info.cache_size = get_cpu_info_impl<cache_size>();
    info.cache_aligment = get_cpu_info_impl<cache_aligment>();

    return info;
  }

  constexpr std::string get_os()
  {
#if defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "UnknownOS";
#endif
  }

  constexpr std::string get_arch()
  {
#if defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
    return "x86";
#elif defined(__aarch64__) || defined(_M_ARM64)
    return "ARM64";
#elif defined(__arm__) || defined(_M_ARM)
    return "ARM";
#else
    return "UnknownArch";
#endif
  }

  constexpr std::string get_simd()
  {
#if defined(__AVX512__)
    return "AVX512";
#elif defined(__AVX2__)
    return "AVX2";
#elif defined(__ARM_NEON__)
    return "NEON";
#else
    return "NoSIMD";
#endif
  }

} // namespace Bin2Chars::Benchmark::SystemInfo
