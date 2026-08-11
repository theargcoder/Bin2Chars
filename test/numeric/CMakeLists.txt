add_executable(test_integers integers.cpp)

target_link_libraries(test_integers PRIVATE Bin2Chars)

add_test(NAME NumericIntegers COMMAND test_integers)

add_subdirectory(floating)
