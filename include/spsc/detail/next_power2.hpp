#pragma once

#include <type_traits>
namespace spsc::detail {
template <typename T>
T next_power2(T n) {
    static_assert(std::is_unsigned_v<T>, "T must be an unsigned type");
    auto p = T{2};
    while (p <= n) {
        p <<= 1;
    }
    return p;
}
}  // namespace spsc::detail
