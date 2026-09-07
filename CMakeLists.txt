cmake_minimum_required(VERSION 3.20)

project(Bin2Chars LANGUAGES C CXX)

# ============================================================
# C / C++ / CMake
# ============================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Build type: Debug, Release, RelWithDebInfo, MinSizeRel"
            FORCE)
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_ARCH)

# ============================================================
# Boost
# ============================================================
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# ============================================================
# Compiler detection
# ============================================================

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(IS_GCC TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(IS_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(IS_MSVC TRUE)
endif()

# ============================================================
# C warnings
#
# These are warnings intended for YOUR C code. They are NOT applied to external
# dependencies.
# ============================================================

set(C_WARNINGS
    # Fundamental
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    # Control flow / logic
    -Wduplicated-cond
    -Wduplicated-branches
    -Wmisleading-indentation
    -Wparentheses
    -Wempty-body
    -Wswitch
    -Wswitch-enum
    # Initialization / lifetime
    -Wuninitialized
    -Wmaybe-uninitialized
    -Winit-self
    -Wreturn-type
    -Wuse-after-free
    # Arrays / bounds
    -Warray-bounds=2
    -Warray-parameter=2
    -Wzero-length-bounds
    -Wsizeof-array-div
    -Wsizeof-pointer-div
    -Wsizeof-pointer-memaccess
    # Pointer / memory correctness
    -Wnull-dereference
    -Wpointer-arith
    -Wcast-align=strict
    -Wcast-qual
    -Wrestrict
    # Integer conversions
    -Wconversion
    -Wsign-conversion
    -Wsign-compare
    -Wchar-subscripts
    # Integer UB
    -Wshift-overflow=2
    -Wshift-negative-value
    -Wdiv-by-zero
    # Floating-point
    -Wfloat-conversion
    -Wfloat-equal
    # String / formatting
    -Wformat=2
    -Wformat-overflow=2
    -Wformat-truncation=2
    -Wformat-security
    -Wstringop-overflow
    -Wstringop-overread
    -Wstringop-truncation
    # Allocation
    -Walloc-zero
    -Walloca
    # Language / declarations
    -Wredundant-decls
    -Wmissing-declarations
    # Undefined / suspicious constructs
    -Wundef
    -Wtrigraphs
    -Wcomment
    -Wmultistatement-macros
    -Wvla
    # Unused
    -Wunused
    -Wunused-parameter
    -Wunused-variable
    -Wno-unused-function
    -Wunused-label
    -Wunused-value
    -Wunused-but-set-variable
    -Wunused-but-set-parameter)

# ============================================================
# C++ warnings
#
# Start with C warnings, then add C++-specific diagnostics.
# ============================================================

set(CXX_WARNINGS
    ${C_WARNINGS}
    # C++ conversions / arithmetic
    -Warith-conversion
    -Wdouble-promotion
    # Aliasing
    -Wstrict-aliasing=3
    # C++ object model
    -Wnon-virtual-dtor
    -Wdelete-non-virtual-dtor
    -Woverloaded-virtual
    -Wold-style-cast
    -Wreorder
    -Wmismatched-tags
    -Wplacement-new=2
    -Wsized-deallocation
    -Wrange-loop-construct
    -Wpessimizing-move
    -Wredundant-move
    -Wself-move
    # Initialization / aggregate issues
    -Wmissing-field-initializers
    -Wmissing-braces
    # C++ declarations / attributes
    -Wmissing-attributes
    -Wmissing-noreturn)

# ============================================================
# GCC-specific warnings
# ============================================================

if(IS_GCC)

  set(GCC_ONLY_WARNINGS
      -Walloc-size -Wstring-compare -Wtype-limits -Wtrampolines -Wframe-address
      -Wvector-operation-performance)

  list(APPEND C_WARNINGS ${GCC_ONLY_WARNINGS})

  list(APPEND CXX_WARNINGS ${GCC_ONLY_WARNINGS})

endif()

# ============================================================
# Clang compatibility
# ============================================================

if(IS_CLANG)

  list(
    REMOVE_ITEM
    C_WARNINGS
    -Wduplicated-cond
    -Wduplicated-branches
    -Wstrict-aliasing=3
    -Warith-conversion
    -Wstringop-overflow
    -Wstringop-overread
    -Wstringop-truncation)

  list(
    REMOVE_ITEM
    CXX_WARNINGS
    -Wduplicated-cond
    -Wduplicated-branches
    -Wstrict-aliasing=3
    -Warith-conversion
    -Wstringop-overflow
    -Wstringop-overread
    -Wstringop-truncation)

endif()

# ============================================================
# Architecture
# ============================================================

set(NATIVE_ARCH_FLAGS)

if(SYSTEM_ARCH MATCHES "^(x86_64|amd64|i386|i686)$")

  if(IS_GCC OR IS_CLANG)
    list(APPEND NATIVE_ARCH_FLAGS -march=native)
  endif()

elseif(SYSTEM_ARCH MATCHES "^(arm64|aarch64)$")

  if(IS_GCC OR IS_CLANG)
    list(APPEND NATIVE_ARCH_FLAGS -mcpu=native)
  endif()

endif()

# ============================================================
# External libraries
#
# IMPORTANT:
#
# Ryu is external/reference code.
#
# DO NOT add project compile options before this point. Anything added with
# add_compile_options() above a child add_subdirectory() can propagate into that
# child.
# ============================================================

add_subdirectory(ryu)

# ============================================================
# Library
# ============================================================

add_library(Bin2Chars INTERFACE)

target_include_directories(Bin2Chars INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Bin2Chars INTERFACE Boost::headers ryu::ryu)

if(IS_GCC OR IS_CLANG)

  # ==========================================================
  # Debug
  #
  # Goal: - Excellent CodeLLDB experience - Maximum diagnostics - Runtime UB
  # detection - Memory corruption detection
  #
  # ==========================================================

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    add_compile_options(
      ${NATIVE_ARCH_FLAGS}
      -O0
      -g3
      # CodeLLDB
      -fno-omit-frame-pointer
      -fno-optimize-sibling-calls
      # Variable tracking
      -fvar-tracking
      -fvar-tracking-assignments
      # Make uninitialized automatic variables obvious
      -ftrivial-auto-var-init=pattern
      # Stack protection
      -fstack-protector-strong
      -fstack-clash-protection
      # Address / UB / leak sanitizers
      -fsanitize=address
      -fsanitize=undefined
      -fsanitize=leak
      # Additional sanitizer coverage
      -fsanitize=bounds-strict
      -fsanitize=alignment
      -fsanitize=object-size
      -fsanitize=pointer-overflow
      -fsanitize=float-cast-overflow
      -fsanitize=float-divide-by-zero
      -fsanitize=bool
      -fsanitize=enum
      -fsanitize=vptr
      -fsanitize=builtin
      # Stop immediately on sanitizer failure
      -fno-sanitize-recover=all
      # Language-specific warnings
      "$<$<COMPILE_LANGUAGE:C>:${C_WARNINGS}>"
      "$<$<COMPILE_LANGUAGE:CXX>:${CXX_WARNINGS}>")

    # --------------------------------------------------------
    # Sanitizer runtime
    # --------------------------------------------------------

    add_link_options(-fsanitize=address -fsanitize=undefined -fsanitize=leak
                     -fno-sanitize-recover=all)

    # ==========================================================
    # Release
    #
    # Goal: Actual performance. Sanitizers deliberately OFF.
    # ==========================================================

  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

    add_compile_options(
      -O3
      -DNDEBUG
      -fno-omit-frame-pointer
      ${NATIVE_ARCH_FLAGS}
      # ------------------------------------------------------
      # Language-specific warnings
      # ------------------------------------------------------
      "$<$<COMPILE_LANGUAGE:C>:${C_WARNINGS}>"
      "$<$<COMPILE_LANGUAGE:CXX>:${CXX_WARNINGS}>")

    if(IS_GCC)

      add_compile_options(-funroll-loops -fprefetch-loop-arrays)

    endif()

    # ==========================================================
    # RelWithDebInfo
    #
    # Optimized but still pleasant in CodeLLDB.
    # ==========================================================

  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

    add_compile_options(
      -O2
      -g3
      -DNDEBUG
      -fno-omit-frame-pointer
      ${NATIVE_ARCH_FLAGS}
      # ------------------------------------------------------
      # Language-specific warnings
      # ------------------------------------------------------
      "$<$<COMPILE_LANGUAGE:C>:${C_WARNINGS}>"
      "$<$<COMPILE_LANGUAGE:CXX>:${CXX_WARNINGS}>")

  endif()

elseif(IS_MSVC)

  # ==========================================================
  # MSVC
  # ==========================================================

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    add_compile_options(/Od /Zi /W4 /WX)

  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

    add_compile_options(/O2 /DNDEBUG /W4 /WX)

  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

    add_compile_options(/O2 /Zi /DNDEBUG /W4 /WX)

  endif()

endif()

# ============================================================
# Static linking of GCC runtimes
#
# Never do this for the Debug sanitizer build.
# ============================================================

if((IS_GCC OR IS_CLANG)
   AND NOT APPLE
   AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")

  add_link_options(-static-libgcc -static-libstdc++)

endif()

# ============================================================
# Tests
#
# This is YOUR source tree. Everything configured above applies here.
# ============================================================

enable_testing()

add_subdirectory(test)
