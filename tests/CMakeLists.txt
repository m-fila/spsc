include(GoogleTest)

add_executable(spsc_queue_test queue_test.cpp)

target_link_libraries(spsc_queue_test PRIVATE GTest::gtest_main spsc)
target_compile_options(
  spsc_queue_test
  PRIVATE -Wall -Werror -Wextra -Wno-interference-size
)

gtest_discover_tests(spsc_queue_test)
