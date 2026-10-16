# CMake include(s).
cmake_minimum_required(VERSION 3.25)
include(FetchContent)

# Tell the user what's happening.
message(STATUS "Building Benchmark as part of the SPSC project")

# Declare where to get Google Benchmark from.
set(
  SPSC_BENCHMARK_SOURCE
  "URL;https://github.com/google/benchmark/archive/refs/tags/v1.10.0.tar.gz;URL_MD5;046d7d6b9a11db09693af9ea2a8c7a82"
  CACHE STRING
  "Source for Google Benchmark, when built as part of this project"
)
mark_as_advanced(SPSC_BENCHMARK_SOURCE)
FetchContent_Declare(Benchmark SYSTEM ${SPSC_BENCHMARK_SOURCE})
# Options used in the build of GoogleBenchmark.
set(
  BENCHMARK_ENABLE_TESTING
  FALSE
  CACHE BOOL
  "Turn off the tests of Google Benchmark"
)
set(
  BENCHMARK_ENABLE_INSTALL
  FALSE
  CACHE BOOL
  "Turn off installing Google Benchmark"
)
set(
  BENCHMARK_INSTALL_TOOLS
  FALSE
  CACHE BOOL
  "Turn off installing Google Benchmark tools"
)
set(
  BENCHMARK_ENABLE_GTEST_TESTS
  FALSE
  CACHE BOOL
  "Turn off fetching GoogleTest by Benchmark"
)
set(
  BENCHMARK_USE_BUNDLED_GTEST
  FALSE
  CACHE BOOL
  "Turn off  GoogleTest bundled with Benchmark"
)
# Get it into the current directory.
FetchContent_MakeAvailable(Benchmark)
