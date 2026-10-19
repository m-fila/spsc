#pragma once

#include <cstddef>
#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

#include "spsc/detail/checked_capacity.hpp"
#include "spsc/detail/wrap_index.hpp"

namespace spsc {
namespace v1 {
/// Single-producer single-consumer bounded queue.
///
/// Baseline version: protects the ring buffer with a mutex.
/// Circular buffer is used to store the items. The buffer is allocated with a
/// size of capacity + 1 to distinguish between full and empty states.
/// Do not destroy the queue while another thread is accessing it.
template <typename T, typename Allocator = std::allocator<T>>
class queue {
    static_assert(std::is_object_v<T> && std::is_same_v<T, std::remove_cv_t<T>>,
                  "T must be a cv-unqualified object type");

    public:
    using value_type = T;
    using allocator_type = Allocator;
    static constexpr std::size_t max_capacity =
        std::numeric_limits<std::size_t>::max() - 1;

    /// Creates an empty queue with the requested capacity.
    /// Requires default-constructible T.
    explicit queue(std::size_t capacity,
                   const Allocator& allocator = Allocator());

    /// Returns the usable queue capacity.
    std::size_t capacity() const noexcept;
    /// Copies an item to the back; returns false if full.
    /// Requires copy-assignable T.
    // mutex-based implementation can throw on lock
    bool try_push(const T& item);
    /// Moves an item to the back; returns false if full.
    /// Requires move-assignable T.
    // mutex-based implementation can throw on lock
    bool try_push(T&& item);
    /// Moves the front item into item; returns false if empty.
    /// Requires move-assignable T.
    // mutex-based implementation can throw on lock
    bool try_pop(T& item);

    private:
    std::mutex m_mtx;
    std::vector<T, Allocator> m_buffer;
    std::size_t m_head = 0u;
    std::size_t m_tail = 0u;

    template <typename U>
    bool try_push_impl(U&& item);
};

template <typename T, typename Allocator>
queue<T, Allocator>::queue(std::size_t capacity, const Allocator& allocator)
    : m_buffer(detail::checked_capacity(capacity, max_capacity) + 1,
               allocator) {
    static_assert(std::is_default_constructible_v<T>,
                  "T must be default constructible");
}

template <typename T, typename Allocator>
std::size_t queue<T, Allocator>::capacity() const noexcept {
    return m_buffer.size() - 1;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(const T& item) {
    return try_push_impl(item);
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(T&& item) {
    return try_push_impl(std::move(item));
}

template <typename T, typename Allocator>
template <typename U>
bool queue<T, Allocator>::try_push_impl(U&& item) {
    auto lock = std::lock_guard(m_mtx);
    // wrap around if needed
    const auto next_head = detail::wrap_index(m_head, m_buffer.size());
    // check if full
    if (next_head == m_tail) {
        return false;
    }
    m_buffer[m_head] = std::forward<U>(item);
    m_head = next_head;
    return true;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_pop(T& item) {
    auto lock = std::lock_guard(m_mtx);
    // check if empty
    if (m_head == m_tail) {
        return false;
    }
    item = std::move(m_buffer[m_tail]);
    // wrap around if needed
    const auto next_tail = detail::wrap_index(m_tail, m_buffer.size());
    m_tail = next_tail;
    return true;
}
}  // namespace v1
}  // namespace spsc
