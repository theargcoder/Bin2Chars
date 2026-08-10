add_executable(test_integers integers.cpp)
target_link_libraries(test_integers PRIVATE OpenLogging ${RYU_LIB})
add_test(NAME NumericIntegers COMMAND test_integers)

add_subdirectory(floating)
