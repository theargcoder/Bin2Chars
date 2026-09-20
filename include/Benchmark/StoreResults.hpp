#pragma once

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "include/Benchmark/SystemInfo.hpp"

#include <boost/type_index.hpp>

#include <CJParse/include/CJParse.hpp>
#include <type_traits>

namespace Bin2Chars::Benchmark::Store
{

  template <typename T>
  constexpr std::string get_pretty_name()
  {
    if constexpr(std::is_same_v<T, std::int8_t>)
      return "int8_t";
    if constexpr(std::is_same_v<T, std::int16_t>)
      return "int16_t";
    if constexpr(std::is_same_v<T, std::int32_t>)
      return "int32_t";
    if constexpr(std::is_same_v<T, std::int64_t>)
      return "int64_t";

    if constexpr(std::is_same_v<T, std::uint8_t>)
      return "uint8_t";
    if constexpr(std::is_same_v<T, std::uint16_t>)
      return "uint16_t";
    if constexpr(std::is_same_v<T, std::uint32_t>)
      return "uint32_t";
    if constexpr(std::is_same_v<T, std::uint64_t>)
      return "uint64_t";

    return boost::typeindex::type_id<T>().pretty_name();
  }

  class File
  {
  private:
    std::filesystem::path path;

  public:
    File() : path(std::filesystem::current_path())
    {
      const size_t MAX_BACKWARDS = 30;
      size_t backwards = 0;

      for(; backwards < MAX_BACKWARDS; backwards++)
      {
        if(path.filename().string() == "Bin2Chars")
        {
          path /= "benchmark/results/" + (Benchmark::SystemInfo::get_os() + "-" + Benchmark::SystemInfo::get_arch() + "-" + Benchmark::SystemInfo::get_simd()) + "/";
          break;
        }

        const auto parent = path.parent_path();
        if(parent == path)
        {
          backwards = MAX_BACKWARDS; // error root reached
          break;
        }
        path = parent;
      }

      if(backwards == MAX_BACKWARDS)
      {
        std::cerr << "current_dir : '" << path << "' \n";
        std::cerr << "couldn't find 'Bin2Chars' directory; ensure to execute this within Bin2Chars directory or a Bin2Chars subdirectory \n blowing up and exiting ..."
                  << std::endl;
        std::terminate();
      }
    };

    template <const char *Benchmark, typename T>
    void Store(const CJParse::CJParse &json, bool EXPONENTIAL_NOTATION)
    {
      std::fstream file;
      const std::string dir = (std::is_integral_v<T>) ? std::string{ "integers" } : std::string{ "floats/" } + ((EXPONENTIAL_NOTATION) ? "exponential" : "decimal");
      const std::string f_name = (std::is_integral_v<T>) ? "all_integers.json" : get_pretty_name<T>() + ".json";

      path /= dir;
      path /= Benchmark;

      std::filesystem::create_directories(path);

      path /= f_name;
      file.open(path, std::ios::out);
      path.remove_filename();
      if constexpr(std::is_floating_point_v<T>)
      {
        for(int i = 0; i < 4; i++)
        {
          path = path.parent_path();
        }
      }
      else
      {
        for(int i = 0; i < 3; i++)
        {
          path = path.parent_path();
        }
      }

      if(file.is_open())
      {
        std::mutex mutex;
        std::scoped_lock lock(mutex);

        file << json.Dump(true);
        file.flush();
        file.close();
      }
      else
      {
        std::cerr << "couldn't open file : '" << path.filename() << "' for writting; \n exiting.." << std::endl;
        std::terminate();
      }
    }
  };

} // namespace Bin2Chars::Benchmark::Store
