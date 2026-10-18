# SPSC

Example implementation of a single-producer single-consumer queue in C++17.

## Getting started

### Requirements

- C++17 compatible compiler
- CMake 3.25 or higher

### Building

To configure and build the project, run the following commands in the root directory of the project:

```sh
cmake -S. -B build
cmake --build build
```

Option `-DSPSC_USE_SYSTEM_LIBS=FALSE` can be used during configuration to build the project with system-installed GoogleTest instead of the one provided in `deps/googletest`.

Option `-DSPSC_BUILD_TESTING=FALSE` can be used during configuration to disable building the tests.

Option `-DSPSC_BUILD_BENCHMARKS=FALSE` can be used during configuration to disable building the benchmarks.

### Testing

After building the project, the test can be run using standard CTest:

```sh
ctest --output-on-failure
```

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
