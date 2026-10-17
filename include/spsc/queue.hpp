#pragma once

#include "spsc/v4/queue.hpp"
#include "spsc/v5/queue.hpp"

namespace spsc {
enum class capacity_policy { exact, minimum };

template <typename T, typename Allocator = std::allocator<T>,
          capacity_policy Policy = capacity_policy::minimum>
class queue;

template <typename T, typename Allocator>
class queue<T, Allocator, capacity_policy::exact>
    : public v4::queue<T, Allocator> {
    public:
    using v4::queue<T, Allocator>::queue;
};

template <typename T, typename Allocator>
class queue<T, Allocator, capacity_policy::minimum>
    : public v5::queue<T, Allocator> {
    public:
    using v5::queue<T, Allocator>::queue;
};

}  // namespace spsc
