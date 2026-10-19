#pragma once

#include "spsc/v6/queue.hpp"
#include "spsc/v7/queue.hpp"

namespace spsc {

/// \brief Capacity policy for the queue
enum class capacity_policy {
    exact,   //< The queue will have fixed capacity exactly equal to the
             // specified
    minimum  //< The queue will have fixed capacity at least equal to the
             // specified, but may be larger
};

/// \brief Single-producer single-consumer bounded queue
///
/// \tparam T Element type of the queue. Must be a cv-unqualified object type.
/// \tparam Allocator Allocator type for the queue.
/// \tparam Policy Capacity policy for the queue.
///
/// This queue is designed for single-producer single-consumer scenarios. It is
/// a bounded queue, meaning that it has a fixed capacity. The capacity is
/// fixed at construction. The exact policy uses the requested capacity; the
/// minimum policy rounds it up to a power of two.
///
/// The queue supports one producer and one consumer accessing it concurrently.
/// Items are appended at the back and removed from the front. The queue should
/// not be destroyed while another thread is accessing it. The queue does not
/// provide an observer for its current size or empty/full state.
///
/// `try_push(const T&)` requires copy-constructible T, and `try_push(T&&)`
/// requires move-constructible T. `try_emplace(args...)` constructs an item
/// from the supplied arguments. `try_pop()` returns an optional and requires
/// move-constructible T; `try_pop(T&)` returns bool and requires
/// move-assignable T. Push and try_emplace return immediately when the queue is
/// full, and pop returns immediately when it is empty.
template <typename T, typename Allocator = std::allocator<T>,
          capacity_policy Policy = capacity_policy::minimum>
class queue;

template <typename T, typename Allocator>
class queue<T, Allocator, capacity_policy::exact>
    : public v6::queue<T, Allocator> {
    public:
    using v6::queue<T, Allocator>::queue;
};

template <typename T, typename Allocator>
class queue<T, Allocator, capacity_policy::minimum>
    : public v7::queue<T, Allocator> {
    public:
    using v7::queue<T, Allocator>::queue;
};

}  // namespace spsc
