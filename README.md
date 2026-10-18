# SPSC

Example implementation of a bounded single-producer single-consumer queue in C++17.

## Getting started

### Requirements

- C++17 compatible compiler
- CMake 3.25 or higher

Additionally, the following dependencies are required to build the tests and benchmarks:
- GTest for unit testing
- Google Benchmark for performance benchmarks

### Building

To configure and build the project, run the following commands in the root directory of the project:

```sh
cmake -S. -B build
cmake --build build
```

The project define several options that can be used during configuration to customize the build:

| Option | Default | Description |
| ------ | ------- | ----------- |
| `SPSC_USE_SYSTEM_LIBS` | OFF | Use system-installed libraries for testing and benchmarking dependencies rather than those bundled in `deps/` |
| `SPSC_BUILD_TESTING` | ON | Enable building the SPSC tests |
| `SPSC_BUILD_BENCHMARKS` | ON | Build the benchmarks |
| `SPSC_BUILD_EXAMPLES` | ON | Build the examples |
| `SPSC_INSTALL` | ON | Enable installation of the SPSC library |
| `SPSC_SANITIZER` | none | Enable sanitizers for the build (address, undefined, thread) |

After building the project, the test can be run using standard CTest:

```sh
ctest --output-on-failure
```


The performance benchmarks are based on Google benchmark. Once built, they can be run on CPU cores 0 and 1 using:
```sh
taskset -c 0,1 ./build/benchmarks/spsc_benchmarks
```



Alternatively, a cmake workflow presets can be used simplify the process of building and running the tests or benchmarks:

```sh
cmake --workflow --preset=default
```

Available presets are:

| Preset    | Description                                                        |
| --------- | ------------------------------------------------------------------ |
| default   | Build and run the tests                                            |
| debug     | Build and run the tests in debug mode                              |
| tsan      | Build and run the tests with thread sanitizer enabled              |
| asan      | Build and run the tests with address sanitizer enabled             |
| ubsan     | Build and run the tests with undefined behaviour sanitizer enabled |
| benchmark | Build and run the benchmarks                                       |

### Usage

This project provides a single-producer single-consumer queue implementation. To use it in different projects, link them against the `SPSC::spsc` target provided by this project and include the header file `spsc/queue.hpp` in your source files.

The following example shows a basic usage :

```cpp
#include <spsc/spsc.hpp>
#include <thread>
#include <iostream>

// Create a new queue with a fixed buffer of at least 4 elements
spsc::queue<int> queue(4);

// Producer
std::thread producer([&queue]() {
    for (int i = 0; i < 10; ++i) {
        while (!queue.try_push(i)) {
            std::this_thread::yield();
        }
        queue.try_push(i);
    }
});

// Consumer
std::thread consumer([&queue]() {
    for (int i = 0; i < 10; ++i) {
        int value = -1;
        while (!queue.try_pop(value)) {
            std::this_thread::yield();
        }
        if (value != i) {
            throw std::runtime_error("Unexpected value popped from queue");
        } else {
            std::cout << "Consumed: " << value << std::endl;
        }
    }
});

```

## Features

### Design

### Trade-offs
