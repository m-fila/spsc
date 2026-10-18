#pragma once

#include <cstddef>
#include <stdexcept>

namespace spsc::detail {

template <typename T>
T checked_capacity(T capacity, T max_capacity) {
    if (capacity == 0 || capacity > max_capacity) {
        throw std::invalid_argument("invalid queue capacity");
    }
    return capacity;
}
}  // namespace spsc::detail
