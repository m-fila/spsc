# CMake include(s).
cmake_minimum_required(VERSION 3.25)
include(FetchContent)

message(STATUS "Building GoogleTest as part of the SPSC project")

# Declare where to get GoogleTest from.
set(
  INTERVIEW_GOOGLETEST_SOURCE
  "URL;https://github.com/google/googletest/archive/refs/tags/v1.18.0.tar.gz;URL_MD5;ae87634285dbe148a7abc6d19c4c05bd"
  CACHE STRING
  "Source for GoogleTest, when built as part of this project"
)
mark_as_advanced(INTERVIEW_GOOGLETEST_SOURCE)
FetchContent_Declare(GoogleTest SYSTEM ${INTERVIEW_GOOGLETEST_SOURCE})
# Options used in the build of GoogleTest.
set(BUILD_GMOCK TRUE CACHE BOOL "Turn off the build of GMock")
set(INSTALL_GTEST FALSE CACHE BOOL "Turn off the installation of GoogleTest")

FetchContent_MakeAvailable(GoogleTest)

# Set up aliases for the GTest targets with the same name that they have when we
# find GTest pre-installed.
if(NOT TARGET GTest::gtest)
  add_library(GTest::gtest ALIAS gtest)
endif()
if(NOT TARGET GTest::gtest_main)
  add_library(GTest::gtest_main ALIAS gtest_main)
endif()
