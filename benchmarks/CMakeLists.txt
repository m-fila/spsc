add_executable(spsc_queue_benchmarks queue_benchmark.cpp)
target_link_libraries(spsc_queue_benchmarks PRIVATE benchmark::benchmark spsc)
target_compile_options(
  spsc_queue_benchmarks
  PRIVATE -Wall -Werror -Wextra -Wno-interference-size
)
