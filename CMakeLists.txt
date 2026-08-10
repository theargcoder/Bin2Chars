cmake_minimum_required(VERSION 3.20)
project(OpenLogging CXX)

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
    # ARM64 Specific Optimizations (Apple Silicon M-series / Linux ARM)
  elseif(SYSTEM_ARCH MATCHES "(arm64|aarch64)")
    add_compile_options(-mcpu=native -funroll-loops -fstrict-aliasing)
    # MSVC Specific Optimizations (Windows)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(SYSTEM_ARCH MATCHES "(x86_64|amd64)")
      add_compile_options(/O2 /arch:AVX512 /DNDEBUG)
    endif()
  endif()
  # GCC specific loop vectorization/prefetching
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
      -fno-omit-frame-pointer
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
      -g
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

    # x86_64 Specific Optimizations (Intel/AMD)
    if(SYSTEM_ARCH MATCHES "(x86_64|amd64|i386|i686)")
      add_compile_options(-march=native -funroll-loops -fstrict-aliasing)
      # GCC specific loop vectorization/prefetching
      if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fprefetch-loop-arrays)
        # Clang specific LTO and vectorization passes
      elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-flto -fvectorize -fslp-vectorize
                            -ffp-contract=fast)
      endif()

      # ARM64 Specific Optimizations (Apple Silicon M-series / Linux ARM)
    elseif(SYSTEM_ARCH MATCHES "(arm64|aarch64)")
      add_compile_options(-mcpu=native -funroll-loops -fstrict-aliasing)
      if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-flto -fvectorize -fslp-vectorize)
      endif()
    endif()

    # MSVC Specific Optimizations (Windows)
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
# Header-only library
# ------------------------------------------------------------
add_library(OpenLogging INTERFACE)

target_include_directories(
  OpenLogging INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../ryu)

# Propagate Boost headers
target_link_libraries(OpenLogging INTERFACE Boost::headers)

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------
enable_testing()
add_subdirectory(test)
