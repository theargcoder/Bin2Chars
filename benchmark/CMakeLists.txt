function(add_bin2chars_executable name src)
  add_executable(${name} ${src})
  target_link_libraries(${name} PRIVATE Bin2Chars)
endfunction()

# ------------------------------------------------------------
# Benchmarks
# ------------------------------------------------------------

add_bin2chars_executable(cpu_cycles_benchmark_ints cpu_cycles_ints.cpp)
add_bin2chars_executable(cpu_cycles_benchmark_floats cpu_cycles_floats.cpp)
