cmake_minimum_required(VERSION 3.20)
project(Bin2CharsMisc CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Boost (header-only usage)
# ------------------------------------------------------------

find_package(Boost REQUIRED)

# ------------------------------------------------------------
# Build type & Architecture Detection
# ------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Choose the type of build." FORCE)
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_ARCH)

# ------------------------------------------------------------
# Compiler warnings & Optimizations
# ------------------------------------------------------------

# Base warnings for all build types (GCC/Clang)

set(COMMON_WARNINGS
    -Wall
    -Wextra
    -Wshadow
    -Wshadow=compatible-local
    -Wshadow=local
    -Werror=shadow
    -Wnull-dereference
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -fno-omit-frame-pointer)

# Strip out GCC-specific warnings if using Clang

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(REMOVE_ITEM COMMON_WARNINGS -Wduplicated-cond -Wduplicated-branches
       -Wlogical-op)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

  # x86_64 Specific Optimizations (Intel/AMD)

  if(SYSTEM_ARCH MATCHES "(x86_64|amd64|i386|i686)")
    add_compile_options(-march=native -funroll-loops -fstrict-aliasing)

    # ARM64 Specific Optimizations

  elseif(SYSTEM_ARCH MATCHES "(arm64|aarch64)")
    add_compile_options(-mcpu=native -funroll-loops -fstrict-aliasing)

    # MSVC Specific Optimizations

  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(SYSTEM_ARCH MATCHES "(x86_64|amd64)")
      add_compile_options(/O2 /arch:AVX512 /DNDEBUG)
    endif()
  endif()

  # GCC specific

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    add_compile_options(
      -fno-omit-frame-pointer
      -g3
      -O0
      -Wall
      -Wshadow
      -Wshadow=compatible-local
      -Wshadow=local
      -Werror=shadow
      -Wnull-dereference
      -Wduplicated-cond
      -Wduplicated-branches
      -Wlogical-op
      -fno-omit-frame-pointer)

  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    add_compile_options(
      -g3
      -O0
      -Wall
      -Wextra
      -Wshadow
      -Werror=shadow
      -Wnull-dereference
      -fno-omit-frame-pointer
      -fvectorize
      -fslp-vectorize
      -ffp-contract=fast)

  endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    add_compile_options(-O3 -DNDEBUG ${COMMON_WARNINGS})

    # x86_64 Specific Optimizations

    if(SYSTEM_ARCH MATCHES "(x86_64|amd64|i386|i686)")

      add_compile_options(-march=native -funroll-loops -fstrict-aliasing)

      # GCC specific

      if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        add_compile_options(-fprefetch-loop-arrays)

        # Clang specific

      elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

        add_compile_options(-flto -fvectorize -fslp-vectorize
                            -ffp-contract=fast)

      endif()

      # ARM64 Specific Optimizations

    elseif(SYSTEM_ARCH MATCHES "(arm64|aarch64)")

      add_compile_options(-mcpu=native -funroll-loops -fstrict-aliasing)

      if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-flto -fvectorize -fslp-vectorize)
      endif()

    endif()

    # MSVC Specific Optimizations

  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    if(SYSTEM_ARCH MATCHES "(x86_64|amd64)")
      add_compile_options(/O2 /arch:AVX512 /DNDEBUG)
    endif()

  endif()

endif()

# ------------------------------------------------------------
# Static Linking of Standard Libraries
# ------------------------------------------------------------

if(MSVC)

  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

  # Ensure we are on Linux/Unix; macOS generally doesn't support static libc++

  if(NOT APPLE)
    add_link_options(-static-libgcc -static-libstdc++)
  endif()

endif()

# ------------------------------------------------------------
# Include paths
# ------------------------------------------------------------

# Bin2Chars headers

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Ryu headers

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../ryu)

# ------------------------------------------------------------
# Ryu library
# ------------------------------------------------------------

if(SYSTEM_ARCH MATCHES "(x86_64|amd64|i386|i686)")

  set(RYU_LIB "/home/lucca/Work/ryu/build/libryu.a")

elseif(SYSTEM_ARCH MATCHES "(arm64|aarch64)")

  set(RYU_LIB "/Users/luccalabattaglia/Work/ryu/build/libryu.a")

endif()

# ------------------------------------------------------------
# Boost
# ------------------------------------------------------------

include_directories(${Boost_INCLUDE_DIRS})

# ------------------------------------------------------------
# Helper function
# ------------------------------------------------------------

function(add_bin2char_misc name src)

  add_executable(${name} ${src})

  target_link_libraries(${name} PRIVATE ${RYU_LIB})

endfunction()

# ------------------------------------------------------------
# Misc experiments
# ------------------------------------------------------------

add_bin2char_misc(table_gen_2_n table_gen_2_n.cpp)
add_bin2char_misc(table_gen_5_n table_gen_5_n.cpp)
add_bin2char_misc(table_prim_2_n table_prim_2_n.cpp)
add_bin2char_misc(table_prim_5_n table_prim_5_n.cpp)
