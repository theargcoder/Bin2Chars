#pragma once
#include <boost/type_index.hpp>

#include <format>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

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

} // namespace Bin2Chars::Tests
