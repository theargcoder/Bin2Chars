function(add_bin2chars_executable name src)
  add_executable(${name} ${src})
  target_link_libraries(${name} PRIVATE Bin2Chars Boost::unit_test_framework)

  # Required on Linux/Unix when dynamically linking against system Boost
  if(NOT WIN32)
    target_compile_definitions(${name} PRIVATE BOOST_TEST_DYN_LINK)
  endif()

  if(BIN2CHARS_CIBUILD)
    target_compile_definitions(${name} PRIVATE BIN2CHARS_CIBUILD)
  endif()

endfunction()

function(add_bin2chars_test name)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

# ------------------------------------------------------------
# Targets & Tests Setup
# ------------------------------------------------------------

add_bin2chars_executable(test_magic_math magic_math.cpp)
add_bin2chars_executable(test_compute_exponent_decimal_expansion
                         compute_exponent_decimal_expansion.cpp)

add_bin2chars_test(test_magic_math)
add_bin2chars_test(test_compute_exponent_decimal_expansion)

add_subdirectory(numeric)
