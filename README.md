# Bin2Chars

**Fast integer and floating-point number → character conversion for C++23.**

Bin2Chars converts numeric values directly into character sequences using specialized integer and floating-point algorithms, with SIMD-optimized paths where applicable.

The project focuses on **low-latency conversion, predictable behavior, avoiding unnecessary work and minimizing/eliminating branching**, with architecture-specific optimizations for x86-64 and ARM64.

For summary benchmark results go to [Performance Results Summary](#performance) section.

> Note that benchmark results are platform specific see [Builds](#builds) for the technicalities.

[![CI](https://github.com/theargcoder/Bin2Chars/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/theargcoder/Bin2Chars/actions/workflows/cmake-multi-platform.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-orange.svg)](LICENSE)

## Features

<dl> <dd>

- Integer-to-characters conversion for all signed and unsigned integers:
  > `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`).
- Floating-point-to-characters conversion for `float` and `double` with arbitrary precision in formats:
  - Exponential notation.
  - Fixed Decimal Expansion.
- No dynamic allocation required by the conversion routines.
- For `integral types`:
  - LUT for fast length calculations.
  - For `x86-64` vector optimizations in AVX-512/AVX2 (if target machine supports it).
  - For `aarch64` (ARM64) also optimized but no vectors.
  - No-branch design for any architecture, to ensure consistent latencies.
- For `floating types`:
  - Compile-time constants and tables for arithmetic-heavy conversion paths.
  - Designed for use in performance-sensitive code where standard formatting facilities may be too expensive.

</dd> </dl>

## Correctness

<dl> <dd>

Correctness is tested continuously through GitHub Actions across multiple compilers, operating systems and build configurations.

The CI matrix includes:

| Platform | Compiler   | Configuration | CI Status                                                                                                                                                                                           |
| -------- | ---------- | ------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Linux    | GCC        | Release       | ![Linux GCC Release](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-ubuntu-latest-GCC-Release.json)              |
| Linux    | GCC        | Debug         | ![Linux GCC Debug](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-ubuntu-latest-GCC-Debug.json)                  |
| Linux    | GCC        | Generic       | ![Linux GCC Generic](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-ubuntu-latest-GCC-Generic.json)              |
| Linux    | Clang      | Release       | ![Linux Clang Release](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-ubuntu-latest-Clang-Release.json)          |
| Linux    | Clang      | Debug         | ![Linux Clang Debug](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-ubuntu-latest-Clang-Debug.json)              |
| Linux    | Clang      | Generic       | ![Linux Clang Generic](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-ubuntu-latest-Clang-Generic.json)          |
| MacOS    | AppleClang | Release       | ![MacOS AppleClang Release](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-macos-latest-AppleClang-Release.json) |
| MacOS    | AppleClang | Debug         | ![MacOS AppleClang Debug](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-macos-latest-AppleClang-Debug.json)     |
| MacOS    | AppleClang | Generic       | ![MacOS AppleClang Generic](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-macos-latest-AppleClang-Generic.json) |
| Windows  | MSVC       | Release       | ![Windows MSVC Release](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-windows-latest-MSVC-Release.json)         |
| Windows  | MSVC       | Generic       | ![Windows MSVC Generic](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/theargcoder/10321f35631b9bc6f87ea49be103fbd3/raw/bin2chars-windows-latest-MSVC-Generic.json)         |

Debug configurations are used for aggressive diagnostics and sanitizer-enabled testing.

The test suite covers:

- Integer conversions:
  - Signed integer types
  - Unsigned integer types
  - SIMD implementations
- Floating-point conversions:
  - Exponential notation
  - Fixed/Decimal notation
- Decimal-exponent table validation (for the table used in floating conversion)
- Integer arithmetic and magic-division algorithms

Always checking against reference implementations, more on that in [Performance and Benchmark Results](#benchmarkingperformance-guidelinines)

</dd> </dl>

## Testing and CI

<dl> <dd>

The CI pipeline builds and executes the test suite on every relevant change.

The goal is simple:

```text
push to /main branch → build → test → sanitize → report
```

A green CI result means the tested platform+platform: configuration, compilation and testing completed successfully without any errors.

</dd> </dl>

## Benchmarking/Performance Guidelinines

<dl> <dd>

Performance comparisons are made against the standard library implementation available on the tested platform, including the relevant `libstdc++`, `libc++`, or MSVC STL implementation.

Also peroformance comparasions are done **exclusively** in Release build type (more on that in [Builds](#builds)).

- Benchmarks in `x86-64` machines are done as specified in [x86_64 - Bin2Chars Benchmark Guidelines](https://github.com/theargcoder/Bin2Chars/tree/main/benchmark#x86-64) leveraging the hardware's `PMU` for perfect cycle-accurate benchmarks.
- Benchmarks in `aarch64` (ARM64) machines are are done as specified in [ARM64 - Bin2Chars Benchmark Guidelines](https://github.com/theargcoder/Bin2Chars/tree/main/benchmark#ARM64) leveraging (when available) the hardware's `PMCCNTR` and `CNTVCT_EL0` for perfect cycle-accurate benchmarks.

</dd> </dl>

<!-- BENCHMARK_SUMMARY:START -->
## Performance
<dl>
   <dd>

Benchmark summaries are generated directly from the benchmark JSON artifacts. Relative performance uses the **median core** measurement.

<details>
<summary><strong>Linux-x86_64-AVX2</strong> — Intel(R) Core(TM) i5-5250U CPU @ 1.60GHz</summary>
<p></p>

<dl>
<dd>

<details>
<summary><strong>CPU information</strong></summary>
<p></p>

| Property | Value |
| :--- | :--- |
| CPU | `Intel(R) Core(TM) i5-5250U CPU @ 1.60GHz` |
| Cache | `3072 KB` |
| Cache alignment | `64` |
| Microcode | `0x2f` |
| Trials | `100000, 25000` |
| Batch size | `1000` |

</details>

<details>
<summary><strong>Integer</strong></summary>
<p></p>

<dl>
<dd>

<a id="benchmark-linux-x86-64-avx2-integer-buffered"></a>
#### Buffered

| Baseline | Relative performance |
| :--- | ---: |
| Standard library | [1.688×](benchmark/README.md#benchmark-linux-x86-64-avx2-integer-buffered) Standard library |

<a id="benchmark-linux-x86-64-avx2-integer-std-string"></a>
#### `std::string`

| Baseline | Relative performance |
| :--- | ---: |
| Standard library | [1.628×](benchmark/README.md#benchmark-linux-x86-64-avx2-integer-std-string) Standard library |

</dd>
</dl>

</details>

<details>
<summary><strong>Floating point</strong></summary>
<p></p>

<dl>
<dd>

<details>
<summary><strong>Decimal notation</strong></summary>
<p></p>

<dl>
<dd>

<a id="benchmark-linux-x86-64-avx2-floating-decimal-buffered"></a>
#### Buffered

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.408×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[1.116×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | [1.404×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[1.150×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | [1.281×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[1.141×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | — | — | — |
| `double` | [0.900×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[0.820×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | [0.886×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[0.807×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | [0.910×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[0.830×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | [0.925×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[0.846×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | [0.899×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Standard library<br>[0.837×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-buffered) Ryu | — |

<a id="benchmark-linux-x86-64-avx2-floating-decimal-std-string"></a>
#### `std::string`

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.347×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[1.106×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | [1.369×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[1.150×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | [1.290×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[1.155×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | — | — | — |
| `double` | [0.922×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[0.840×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | [0.907×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[0.828×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | [0.927×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[0.844×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | [0.929×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[0.852×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | [0.925×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Standard library<br>[0.864×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-decimal-std-string) Ryu | — |

</dd>
</dl>

</details>

<details>
<summary><strong>Exponential notation</strong></summary>
<p></p>

<dl>
<dd>

<a id="benchmark-linux-x86-64-avx2-floating-exponential-buffered"></a>
#### Buffered

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.511×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[1.215×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | [1.489×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[1.247×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | [1.540×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[1.432×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | — | — | — |
| `double` | [1.445×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[1.119×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | [1.369×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[1.104×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | [1.117×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[0.983×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | [1.048×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[0.975×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | [1.037×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Standard library<br>[1.004×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-buffered) Ryu | — |

<a id="benchmark-linux-x86-64-avx2-floating-exponential-std-string"></a>
#### `std::string`

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.374×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[1.165×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | [1.404×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[1.205×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | [1.497×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[1.395×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | — | — | — |
| `double` | [1.358×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[1.112×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | [1.339×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[1.112×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | [1.118×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[0.988×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | [1.053×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[0.983×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | [1.069×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Standard library<br>[1.032×](benchmark/README.md#benchmark-linux-x86-64-avx2-floating-exponential-std-string) Ryu | — |

</dd>
</dl>

</details>

</dd>
</dl>

</details>

</dd>
</dl>

[View complete benchmark data →](benchmark/README.md)

</details>


---

<details>
<summary><strong>Linux-x86_64-AVX512</strong> — AMD Ryzen 5 9600X 6-Core Processor</summary>
<p></p>

<dl>
<dd>

<details>
<summary><strong>CPU information</strong></summary>
<p></p>

| Property | Value |
| :--- | :--- |
| CPU | `AMD Ryzen 5 9600X 6-Core Processor` |
| Cache | `1024 KB` |
| Cache alignment | `64` |
| Microcode | `0xb404035` |
| Trials | `100000, 25000` |
| Batch size | `1000` |

</details>

<details>
<summary><strong>Integer</strong></summary>
<p></p>

<dl>
<dd>

<a id="benchmark-linux-x86-64-avx512-integer-buffered"></a>
#### Buffered

| Baseline | Relative performance |
| :--- | ---: |
| Standard library | [2.836×](benchmark/README.md#benchmark-linux-x86-64-avx512-integer-buffered) Standard library |

<a id="benchmark-linux-x86-64-avx512-integer-std-string"></a>
#### `std::string`

| Baseline | Relative performance |
| :--- | ---: |
| Standard library | [2.310×](benchmark/README.md#benchmark-linux-x86-64-avx512-integer-std-string) Standard library |

</dd>
</dl>

</details>

<details>
<summary><strong>Floating point</strong></summary>
<p></p>

<dl>
<dd>

<details>
<summary><strong>Decimal notation</strong></summary>
<p></p>

<dl>
<dd>

<a id="benchmark-linux-x86-64-avx512-floating-decimal-buffered"></a>
#### Buffered

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.699×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[1.369×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | [1.645×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[1.365×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | [1.566×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[1.401×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | — | — | — |
| `double` | [0.960×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[0.879×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | [0.962×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[0.875×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | [0.985×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[0.900×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | [0.969×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[0.902×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | [0.958×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Standard library<br>[0.919×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-buffered) Ryu | — |

<a id="benchmark-linux-x86-64-avx512-floating-decimal-std-string"></a>
#### `std::string`

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.428×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[1.200×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | [1.467×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[1.266×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | [1.447×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[1.324×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | — | — | — |
| `double` | [0.938×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[0.871×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | [0.916×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[0.853×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | [0.958×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[0.893×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | [0.963×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[0.903×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | [0.958×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Standard library<br>[0.917×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-decimal-std-string) Ryu | — |

</dd>
</dl>

</details>

<details>
<summary><strong>Exponential notation</strong></summary>
<p></p>

<dl>
<dd>

<a id="benchmark-linux-x86-64-avx512-floating-exponential-buffered"></a>
#### Buffered

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.590×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.310×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | [1.524×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.280×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | [1.692×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.542×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | — | — | — |
| `double` | [1.492×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.163×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | [1.447×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.173×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | [1.267×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.115×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | [1.108×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.037×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | [1.080×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Standard library<br>[1.051×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-buffered) Ryu | — |

<a id="benchmark-linux-x86-64-avx512-floating-exponential-std-string"></a>
#### `std::string`

| Type | 0–10 | 11–30 | 31–100 | 101–300 | 301–500 | 501+ |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: |
| `float` | [1.456×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.233×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | [1.442×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.248×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | [1.600×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.468×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | — | — | — |
| `double` | [1.391×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.170×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | [1.345×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.154×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | [1.183×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.063×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | [1.052×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[0.985×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | [1.064×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Standard library<br>[1.037×](benchmark/README.md#benchmark-linux-x86-64-avx512-floating-exponential-std-string) Ryu | — |

</dd>
</dl>

</details>

</dd>
</dl>

</details>

</dd>
</dl>

[View complete benchmark data →](benchmark/README.md)

</details>
<!-- BENCHMARK_SUMMARY:END -->

---

> Click on the '>' above to expand the result tables for the platform of your interest

More of the theory and detailed procedures for the benchmarks can be found in [benchmark](https://github.com/theargcoder/Bin2Chars/tree/main/benchmark) dir.

As well as the detailed documentation with references on methodologies [Documentation, Methodology and References](https://github.com/theargcoder/Bin2Chars/tree/main/benchmark#performance-and-benchmark-results)

  </dd>
</dl>

## Builds

<dl> <dd>

This library supports the following build types, expand each section for more information on it:

<dl> <dd>

<details>
<summary><code>Release</code></summary>

<p></p>

<dl> <dd>

The Release build type focuses on maximizing performance by compiling code to native architecture, it relies heavy on my manually coded SIMD (AVX-512 or AVX2) or Optimized conversion paths.

The compilation flags are:
<details>
  <summary> <code>List of compilation flags</code> </summary>
  <dl> <dd>
Options:

- O3
- fno-omit-frame-pointer
- march=native (or in clang : mcpu=native)
- funroll-loops
- prefetch-loop-arrays

  </dd> </dl>

</details>

</dd> </dl>

</details>

<details>
<summary><code>Debug</code></summary>
<p></p>

<dl> <dd>
The Debug pipeline is intentionally much more aggressive than a normal development build.

The compilation flags are:
<details>
  <summary> <code>List of compilation flags</code> </summary>
  <dl> <dd>
Options:

- O0
- g3
- fno-omit-frame-pointer
- fno-optimize-sibling-calls
- fno-sanatize-recover=all
- march=native (or in clang : mcpu=native)

  </dd> </dl>

</details>

It is used to exercise the library with runtime diagnostics, listed in the below section:

<details>
  <summary><code>List of Runtime Diagnostics</code></summary>
  <dl> <dd>
Sanatize:

- Address
- address
- undefined
- leak
- bounds-strict
- alignment
- object-size
- pointer-overflow
- float-cast-overflow
- float-divide-by-zero
- bool
- enum
- vptr
- builtin
  </dd> </dl>

</details>

The purpose of the Debug pipeline is not performance; It is to make undefined behavior, memory errors, alignment mistakes, invalid conversions, and other implementation bugs extremely difficult to hide.

Debug builds also have `-march=native` flags to catch any UB, Sanatize and friends stuff happening within vectorizations.

</dd> </dl>

</details>

<details>
<summary><code>RelWithDebInfo</code></summary>

<p></p>

<dl> <dd>

The RelWithDebInfo build type focuses on compiling with max optimizations including the native manually coded SIMD/Optimized paths

Useful for profiling or benchmarking tools

The compilation flags are:
<details>
  <summary> <code>List of compilation flags</code> </summary>
  <dl> <dd>
Options:

- O3
- g3
- fno-omit-frame-pointer
- march=native (or in clang : mcpu=native)

  </dd> </dl>

</details>
  </dd> </dl>

</details>

<details>
<summary><code>Generic</code></summary>

<p></p>

<dl> <dd>

The Generic build type focuses on compiling with max optimizations but for generic architectures so no SIMD here.

Useful for ABIs or when needing portability.

The compilation flags are:
<details>
  <summary><code>List of compilation flags</code></summary>
  <dl> <dd>
Options:

- O3
- mtune=generic
- march=x86_64 or armv8-a

  </dd> </dl>

</details>

  </dd> </dl>

</details>

  </dd>
</dl>

This can be specified when building, check [How to build it](#how-to-build-it) for more info.

Every build features all the Static Analysis checks supported by the used compiler

> CMake will automatically check and remove warnings that are not supported by the compiler, this was done by design.

Here are most of them:

<details>
  <summary> <code>List of Static Analysis Checks</code></summary>
  <dl> <dd>
Checks:

- -Wall -Wextra -Wpedantic -Werror
- Control flow / logic
  - -Wduplicated-cond -Wduplicated-branches -Wmisleading-indentation -Wparentheses -Wempty-body -Wswitch -Wswitch-enum
- Initialization / lifetime
  - -Wuninitialized -Wmaybe-uninitialized -Winit-self -Wreturn-type -Wuse-after-free
- Arrays / bounds
  - -Warray-bounds=2 -Warray-parameter=2 -Wzero-length-bounds -Wsizeof-array-div -Wsizeof-pointer-div -Wsizeof-pointer-memaccess
- Pointer / memory correctness
  - -Wnull-dereference -Wpointer-arith -Wcast-align=strict -Wcast-qual -Wrestrict
- Integer conversions
  - -Wconversion -Wsign-conversion -Wsign-compare -Wchar-subscripts
- Integer UB
  - -Wshift-overflow=2 -Wshift-negative-value -Wdiv-by-zero
- Floating-point
  - -Wfloat-conversion -Wfloat-equal
- String / formatting
  - -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Wformat-security -Wstringop-overflow -Wstringop-overread -Wstringop-truncation
- Allocation
  - -Walloc-zero -Walloca
- Language / declarations
  - -Wredundant-decls -Wmissing-declarations
- Undefined / suspicious constructs
  - -Wundef -Wtrigraphs -Wcomment -Wmultistatement-macros -Wvla
- Unused
  - -Wunused -Wunused-parameter -Wunused-variable -Wno-unused-function -Wunused-label -Wunused-value -Wunused-but-set-variable -Wunused-but-set-parameter

    </dd> </dl>

</details>

Compiler warnings are ERRORS, this is to ensure no language specific / implementation bugs appear.

  </dd>
</dl>

## Algorithms

<dl> <dd>

Bin2Chars does not rely on a single generic conversion routine.

Different numeric categories use specialized algorithms.

### Integers

<dl> <dd>
Integer conversion uses combinations of:

- Multiplication by precomputed constants
- Shift-based division
- Magic-number division (aka Hackers Delight chap. 10.)
- LUTs for length calculations
- SIMD (when architecture has it available)

The implementation is particularly focused on avoiding expensive division operations in the hot path.

</dd> </dl>

### Floating point

<dl> <dd>

Floating-point conversion is split into separate algorithmic stages for:

- Exponent computation
- Decimal expansion
- Exponential notation
- Significant-digit generation
- Rounding and digit selection

The implementation uses integer arithmetic and precomputed powers/constants to transform binary floating-point values into the CORRECT decimal representations.

</dd> </dl>

  </dd>
</dl>

## Reference Implementations

<dl> <dd>

Correctness is checked against established implementations rather than relying solely on Bin2Chars' own algorithms.

This means that we are trusting that std::libs implementations are CORRECT, and checking results against that.

The test suite includes comparisons against standard library formatting and the bundled Ryu implementation where appropriate.

A conversion is considered correct only when `std::string == operator` (aka strcmp) returns true; meaning we generated THE SAME CHARACTERS in SAME ORDER as std::lib.

  </dd>
</dl>

## How to Build it

<dl>
  <dd>

Bin2Chars uses CMake and requires C++23 (for now).

A typical build is:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

For a diagnostic/debug build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

The project also exports the compilation database for tooling such as `clangd`.

</dd> </dl>

## Testing

<dl> <dd>

The repository contains tests for the individual algorithmic components as well as the complete conversion pipeline.

includes:

```text
test/exp_table.cpp
test/magic_math.cpp

test/numeric/integers.cpp

test/numeric/floating/decimal_expansion.cpp
test/numeric/floating/exponential_notation.cpp
```

These tests cover both algorithmic correctness and low-level implementation details.

</dd> </dl>

## CI

<dl> <dd>

GitHub Actions is used to continuously validate the project.

The CI configuration tests multiple combinations of:

```text
              ┌──────── GCC
              │
              ├──────── Clang
Source ───────┤
              └──────── MSVC
                   │
                   ├── Debug
                   └── Release
```

The Debug jobs prioritize diagnostics, runtime checking, and sanitizer coverage.

The Release jobs validate optimized compilation and the normal production configuration.

This catches a different class of problems than a single local compiler build:

- Compiler-specific diagnostics
- MSVC portability issues
- GCC/Clang differences
- Optimization-sensitive bugs
- Undefined behavior
- Architecture-dependent code paths
- Template instantiation problems
- SIMD implementation issues

The current workflow is:

[`workflow.yml`](.github/workflows/cmake-multi-platform.yml)

</dd> </dl>

## Design Goals

<dl> <dd>

Bin2Chars is built around a few simple goals:

**Low latency**

<dl> <dd> Conversion should spend as few instructions as practical without branching to ensure consistent and well defined latency in CPU cycles for any given architecture </dd> </dl>

**Correctness**

<dl> <dd> Optimizations are only useful when the resulting character representation is correct. </dd> </dl>

**Architecture awareness**

<dl> <dd> The implementation should make use of the hardware it runs on without forcing every path to use the same instruction set. </dd> </dl>

**Reproducible testing**

<dl> <dd> Changes should be validated continuously across multiple compilers and configurations. </dd> </dl>

**Minimal unnecessary work**

<dl> <dd> Avoid divisions, branches, memory traffic, and intermediate representations when specialized arithmetic can do the job directly. </dd> </dl>

</dd> </dl>

## Status

<dl> <dd>

Bin2Chars is fully functional library and you can feel free to use it!!

</dd> </dl>

## License

Bin2Chars is released under the GPLv3.0 License.

See [LICENSE](LICENSE).
