# Set library path for the linker
if("${SYSTEM_ARCH}" MATCHES "(x86_64|amd64|i386|i686)")
    set(RYU_LIB "/home/lucca/Work/ryu/build/libryu.a")
elseif("${SYSTEM_ARCH}" MATCHES "(arm64|aarch64)")
    set(RYU_LIB "/Users/luccalabattaglia/Work/ryu/build/libryu.a")
endif()

# Helper function to reduce "CMake shit"
function(add_logging_test name src)
  add_executable(${name} ${src})
  # This inherits SIMD flags, Boost, and Ryu/OpenLogging includes automatically
  target_link_libraries(${name} PRIVATE OpenLogging ${RYU_LIB})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_logging_test(test_constants constants.cpp)
add_logging_test(test_magic_math magic_math.cpp)
add_logging_test(test_asm rand_asm_test.cpp)

add_subdirectory(numeric)
