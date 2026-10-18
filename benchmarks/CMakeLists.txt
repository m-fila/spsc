add_executable(
  spsc_benchmarks
  # common files
  queue_benchmark_common.hpp
  # benchmarks
  v1_queue_benchmark.cpp
  v2_queue_benchmark.cpp
  v3_queue_benchmark.cpp
  v4_queue_benchmark.cpp
  v5_queue_benchmark.cpp
  queue_benchmark.cpp
)
target_link_libraries(spsc_benchmarks PRIVATE benchmark::benchmark_main spsc)
spsc_target_compile_options(spsc_benchmarks)

add_custom_target(
  bench
  COMMAND taskset -c 0,1 "${CMAKE_CURRENT_BINARY_DIR}/spsc_benchmarks"
  DEPENDS spsc_benchmarks
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
