# ------------------------------------------------------------
# Exponential notation
# ------------------------------------------------------------
add_executable(test_exponential_notation exponential_notation.cpp)
target_link_libraries(test_exponential_notation PRIVATE Bin2Chars)
add_test(NAME FloatingExponential COMMAND test_exponential_notation)

# ------------------------------------------------------------
# N-digits precision
# ------------------------------------------------------------
add_executable(test_n_digits_precision n_digits_precision.cpp)
target_link_libraries(test_n_digits_precision PRIVATE Bin2Chars)
add_test(NAME FloatingPrecision COMMAND test_n_digits_precision)
