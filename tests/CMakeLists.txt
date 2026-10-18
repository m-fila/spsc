include(GoogleTest)

add_executable(
  spsc_tests
  # common files
  counting_memory_resource.hpp
  custom_types.hpp
  queue_test_common.hpp
  # tests
  detail_bit_test.cpp
  detail_storage_test.cpp
  detail_wrap_index_test.cpp
  v1_queue_test.cpp
  v2_queue_test.cpp
  v3_queue_test.cpp
  v4_queue_test.cpp
  v5_queue_test.cpp
  v6_queue_test.cpp
  v7_queue_test.cpp
  queue_test.cpp
)

target_link_libraries(spsc_tests PRIVATE GTest::gtest_main spsc)
spsc_target_compile_options(spsc_tests)

gtest_discover_tests(spsc_tests)

if(SPSC_BUILD_EXAMPLES)
  add_test(NAME spsc_examples_test COMMAND spsc_example)
endif()
