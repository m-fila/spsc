add_executable(spsc_benchmarks queue_benchmark.cpp)
target_link_libraries(spsc_benchmarks PRIVATE benchmark::benchmark_main spsc)
target_compile_options(
  spsc_benchmarks
  PRIVATE -Wall -Werror -Wextra -Wno-interference-size
)

add_custom_target(
  bench
  COMMAND taskset -c 0,1 "${CMAKE_CURRENT_BINARY_DIR}/spsc_benchmarks"
  DEPENDS spsc_benchmarks
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
