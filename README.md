# SPSC

Example implementation of a bounded single-producer single-consumer queue in C++17.

This project contains several queue implementations (`spsc::v1::queue`,
`spsc::v2::queue` etc.) that show successive stages of optimization and interface changes. For general
use, prefer `spsc::queue` from `spsc/queue.hpp`; its default minimum-capacity
policy uses the final implementation, `spsc::v7::queue`. The exact-capacity
policy uses `spsc::v6::queue`.

## Features

The library provides a bounded single-producer single-consumer queue. The queue was designed to be efficient while staying generic enough to support a wide range of types. The queue is implemented as a circular buffer with capacity fixed at construction time and cannot be changed afterwards. A compile-time policy can be used to select whether the queue's capacity will be exact to the requested size or rounded up to the next power of two. The capacity must be greater than zero and smaller or equal to `max_capacity`. The queue accepts an allocator to customize the memory allocation strategy.

`T` must be a cv-unqualified object type; `void`, references and `const`/`volatile`-qualified types are not valid queue element types.

Items are appended at the back and removed from the front. `try_push(const T&)` copies an item into the queue and requires `T` to be copy-constructible; `try_push(T&&)` moves an item into the queue and requires `T` to be move-constructible. `try_emplace(args...)` constructs an item directly from the supplied arguments. Push and emplace return `false` when the queue is full.

`try_pop()` returns a `std::optional<T>` containing the front item, or an empty optional when the queue is empty; it requires `T` to be move-constructible. `try_pop(T&)` moves the front item into the supplied object and returns `true`, or returns `false` when the queue is empty; it requires `T` to be move-assignable.

Element construction or transfer can throw but should leave the elements in a valid state. If an operation throws, the queue indices are not advanced, so the queued item is not removed.

The queue supports one producer thread performing `try_push` or `try_emplace` concurrently with one consumer thread performing either `try_pop` overload. It does not support multiple producers or multiple consumers. The producer and consumer must externally coordinate shutdown; destroy the queue only after no thread can access it. The queue does not provide an observer for its current size or empty/full state. Its `capacity()` remains constant throughout its lifetime.

## Requirements

- C++17 compatible compiler
- CMake 3.25 or higher

Additionally, the following dependencies are required to build the tests and benchmarks:

- GTest for unit testing
- Google Benchmark for performance benchmarks

## Building

To configure and build the project, run the following commands in the root directory of the project:

```sh
cmake -S . -B build
cmake --build build
```

The project defines several options that can be used during configuration to customize the build:

| Option                  | Default | Description                                                                                                   |
| ----------------------- | ------- | ------------------------------------------------------------------------------------------------------------- |
| `SPSC_USE_SYSTEM_LIBS`  | OFF     | Use system-installed libraries for testing and benchmarking dependencies rather than those bundled in `deps/` |
| `SPSC_BUILD_TESTING`    | ON      | Enable building the SPSC tests                                                                                |
| `SPSC_BUILD_BENCHMARKS` | ON      | Build the benchmarks                                                                                          |
| `SPSC_BUILD_EXAMPLES`   | ON      | Build the examples                                                                                            |
| `SPSC_INSTALL`          | ON      | Enable installation of the SPSC library                                                                       |
| `SPSC_SANITIZER`        | none    | Enable sanitizers for the build (address, undefined, thread)                                                  |

After building the project, the test can be run using standard CTest:

```sh
ctest --test-dir build
```

The performance benchmarks are based on Google benchmark. Once built, they can be run on CPU cores 0 and 1 using:

```sh
taskset -c 0,1 ./build/benchmarks/spsc_benchmarks
```

Alternatively, a CMake workflow preset can be used simplify the process of building and running the tests or benchmarks:

```sh
cmake --workflow --preset=default
```

The available presets are:

| Preset    | Description                                                |
| --------- | ---------------------------------------------------------- |
| default   | Build and run the tests                                    |
| debug     | Build in debug mode and run the tests in debug mode        |
| tsan      | Build with thread sanitizer and run the tests              |
| asan      | Build with address sanitizer and run the tests             |
| ubsan     | Build with undefined behaviour sanitizer and run the tests |
| benchmark | Build and run the benchmarks                               |
