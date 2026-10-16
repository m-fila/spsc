include(GoogleTest)

add_executable(spsc_tests queue_test.cpp)

target_link_libraries(spsc_tests PRIVATE GTest::gtest_main spsc)
target_compile_options(
  spsc_tests
  PRIVATE -Wall -Werror -Wextra -Wno-interference-size
)

gtest_discover_tests(spsc_tests)
