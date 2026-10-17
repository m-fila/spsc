cmake_minimum_required(VERSION 3.25)
project(
  "SPSC"
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "Example single-producer single-consumer bounded queue"
)

## General options

option(SPSC_BUILD_TESTING "Build the tests of SPSC" TRUE)
option(SPSC_BUILD_BENCHMARKS "Build the benchmarks of SPSC" TRUE)
option(
  SPSC_USE_SYSTEM_LIBS
  "Use system libraries instead of building them as part of this project"
  FALSE
)
option(SPSC_INSTALL "Install the SPSC library" TRUE)

include(cmake/Sanitizers.cmake) # SPSC_SANITIZER option

## Library

add_library(spsc INTERFACE)
target_compile_features(spsc INTERFACE cxx_std_17)
target_sources(
  spsc
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include/ "${CMAKE_CURRENT_BINARY_DIR}/include"
      FILES
        include/spsc/spsc.hpp
        include/spsc/queue.hpp
        include/spsc/v1/queue.hpp
        include/spsc/v2/queue.hpp
        include/spsc/v3/queue.hpp
        include/spsc/v4/queue.hpp
        include/spsc/v5/queue.hpp
        include/spsc/detail/bit.hpp
        include/spsc/detail/macros.hpp
)
spsc_apply_sanitizer(spsc)
add_library(SPSC::spsc ALIAS spsc)

## Tests

### Dependencies
### Set up GoogleTest either by using a system installation or by building it as part of this project.

include(CTest)
if((BUILD_TESTING AND SPSC_BUILD_TESTING) OR SPSC_BUILD_BENCHMARKS)
  set(SPSC_DEFAULT_SETUP_GOOGLETEST TRUE)
endif()
option(
  SPSC_SETUP_GOOGLETEST
  "Set up the GoogleTest target(s) explicitly"
  ${SPSC_DEFAULT_SETUP_GOOGLETEST}
)
if(SPSC_USE_SYSTEM_LIBS)
  set(SPSC_USE_SYSTEM_GOOGLETEST_DEFAULT ON)
else()
  set(SPSC_USE_SYSTEM_GOOGLETEST_DEFAULT OFF)
endif()
option(
  SPSC_USE_SYSTEM_GOOGLETEST
  "Pick up an existing installation of GoogleTest from the build environment"
  ${SPSC_USE_SYSTEM_GOOGLETEST_DEFAULT}
)
unset(SPSC_USE_SYSTEM_GOOGLETEST_DEFAULT)
if(SPSC_SETUP_GOOGLETEST)
  if(SPSC_USE_SYSTEM_GOOGLETEST)
    find_package(GTest REQUIRED)
  else()
    add_subdirectory(deps/googletest)
  endif()
endif()

if(BUILD_TESTING AND SPSC_BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

## Benchmarks
### Dependencies
### Set up Google benchmark either by using a system installation or by building it as part of this project.

if(SPSC_BUILD_BENCHMARKS)
  set(SPSC_DEFAULT_SETUP_BENCHMARK TRUE)
endif()
option(
  SPSC_SETUP_BENCHMARK
  "Set up the Google benchmark target(s) explicitly"
  ${SPSC_DEFAULT_SETUP_BENCHMARK}
)
if(SPSC_USE_SYSTEM_LIBS)
  set(SPSC_USE_SYSTEM_BENCHMARK_DEFAULT ON)
else()
  set(SPSC_USE_SYSTEM_BENCHMARK_DEFAULT OFF)
endif()
option(
  SPSC_USE_SYSTEM_BENCHMARK
  "Pick up an existing installation of Google benchmark from the build environment"
  ${SPSC_USE_SYSTEM_BENCHMARK_DEFAULT}
)
unset(SPSC_USE_SYSTEM_BENCHMARK_DEFAULT)
if(SPSC_SETUP_BENCHMARK)
  if(SPSC_USE_SYSTEM_BENCHMARK)
    find_package(Benchmark REQUIRED)
  else()
    add_subdirectory(deps/benchmark)
  endif()
endif()

if(SPSC_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

## Install
if(SPSC_INSTALL)
  install(TARGETS spsc EXPORT SPSCTargets FILE_SET HEADERS)

  install(
    EXPORT SPSCTargets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SPSC"
    NAMESPACE SPSC::
  )

  include(CMakePackageConfigHelpers)

  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SPSCConfigVersion.cmake"
    COMPATIBILITY ExactVersion
  )

  install(
    FILES
      cmake/SPSCConfig.cmake
      "${CMAKE_CURRENT_BINARY_DIR}/SPSCConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SPSC"
  )
endif()
