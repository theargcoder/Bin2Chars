# Exponential Notation
add_executable(test_exponential_notation exponential_notation.cpp)
target_link_libraries(test_exponential_notation PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME FloatingExponential COMMAND test_exponential_notation)

# N-Digits Precision
add_executable(test_n_digits_precision n_digits_precision.cpp)
target_link_libraries(test_n_digits_precision PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME FloatingPrecision COMMAND test_n_digits_precision)
