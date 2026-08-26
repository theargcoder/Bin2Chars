# ------------------------------------------------------------
# Helper function
# ------------------------------------------------------------
function(add_bin2chars_test name src)

  add_executable(${name} ${src})

  target_link_libraries(${name} PRIVATE Bin2Chars)

  add_test(NAME ${name} COMMAND ${name})

endfunction()

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------

add_bin2chars_test(test_constants constants.cpp)
add_bin2chars_test(test_magic_math magic_math.cpp)
add_bin2chars_test(test_asm rand_asm_test.cpp)
add_bin2chars_test(test_compute_decimal_expansion compute_decimal_expansion.cpp)

add_subdirectory(numeric)
