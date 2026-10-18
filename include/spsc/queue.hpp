#pragma once

#include "spsc/v4/queue.hpp"
#include "spsc/v5/queue.hpp"

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
/// \tparam T Element type of the queue. Must be default constructible and
/// should be assignable for the push and pop operations to work correctly.
/// \tparam Allocator Allocator type for the queue.
/// \tparam Policy Capacity policy for the queue.
///
/// This queue is designed for single-producer single-consumer scenarios. It is
/// a bounded queue, meaning it has a fixed capacity. The capacity can be
/// specified at construction time, and the queue will not grow beyond this
/// capacity. The queue supports two capacity policies: exact and minimum. The
/// exact policy ensures that the queue has a fixed capacity exactly equal to
/// the specified value, while the minimum policy allows the queue to have a
/// fixed capacity at least equal to the specified value.
///
/// The queue is implemented using a circular buffer. The queue is thread-safe
/// for a single producer and a single consumer, meaning that one thread can
/// safely push items into the queue while another thread pops items from the
/// queue without additional synchronization.
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
