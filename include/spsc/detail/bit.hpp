#pragma once

#include <limits>
#include <stdexcept>
#include <type_traits>
namespace spsc::detail {

template <typename T>
constexpr T max_power2() {
    static_assert(std::is_unsigned_v<T>, "T must be an unsigned type");
    return T{1} << (std::numeric_limits<T>::digits - 1);
}

/// Returns the next power of 2 greater than n. If n is already a power of 2,
/// returns the next power of 2.
template <typename T>
T next_power2(T n) {
    static_assert(std::is_unsigned_v<T>, "T must be an unsigned type");
    const auto max_value = max_power2<T>();
    if (n >= max_value) {
        throw std::overflow_error(
            "the input value is too large to compute the next power of 2");
    }
    auto p = T{2};
    while (p <= n) {
        p <<= 1;
    }
    return p;
}
}  // namespace spsc::detail
