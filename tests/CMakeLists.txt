include(GoogleTest)

add_executable(
  spsc_tests
  # common files
  counting_memory_resource.hpp
  custom_types.hpp
  queue_test_common.hpp
  # tests
  detail_bit_test.cpp
  v1_queue_test.cpp
  v2_queue_test.cpp
  v3_queue_test.cpp
  v4_queue_test.cpp
  v5_queue_test.cpp
)

target_link_libraries(spsc_tests PRIVATE GTest::gtest_main spsc)
target_compile_options(
  spsc_tests
  PRIVATE -Wall -Werror -Wextra -Wno-interference-size
)

gtest_discover_tests(spsc_tests)
