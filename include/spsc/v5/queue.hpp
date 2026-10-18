#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#include "spsc/detail/bit.hpp"
#include "spsc/detail/checked_capacity.hpp"

namespace spsc {
namespace v5 {
/// Single-producer single-consumer bounded queue.
///
/// Based on v4 but uses power-of-two capacity and monotonic indices for
/// mask-based indexing. The capacity will be automatically rounded up to the
/// next power of two based on the specified value.
/// Do not destroy the queue while another thread is accessing it.
template <typename T, typename Allocator = std::allocator<T>>

class queue {
    public:
    using value_type = T;
    using allocator_type = Allocator;
    static constexpr std::size_t max_capacity =
        detail::max_power2<std::size_t>();

    /// Creates an empty queue with capacity rounded up to a power of two.
    /// Requires default-constructible T.
    explicit queue(std::size_t capacity,
                   const Allocator& allocator = Allocator());

    /// Returns the usable queue capacity.
    std::size_t capacity() const noexcept;
    /// Copies an item to the back; returns false if full.
    /// Requires copy-assignable T.
    bool try_push(const T& item) noexcept(std::is_nothrow_copy_assignable_v<T>);
    /// Moves an item to the back; returns false if full.
    /// Requires move-assignable T.
    bool try_push(T&& item) noexcept(std::is_nothrow_move_assignable_v<T>);
    /// Moves the front item into item; returns false if empty.
    /// Requires move-assignable T.
    bool try_pop(T& item) noexcept(std::is_nothrow_move_assignable_v<T>);

    private:
    std::vector<T, Allocator> m_buffer;
    const std::size_t m_mask;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_head = 0u;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_tail = 0u;
    alignas(std::hardware_destructive_interference_size) std::size_t
        m_saved_head = 0u;
    alignas(std::hardware_destructive_interference_size) std::size_t
        m_saved_tail = 0u;

    template <typename U>
    bool try_push_impl(U&& item) noexcept(
        std::is_nothrow_assignable_v<T&, U&&>);
};

template <typename T, typename Allocator>
queue<T, Allocator>::queue(std::size_t minimal_capacity,
                           const Allocator& allocator)
    : m_buffer(spsc::detail::next_power2(spsc::detail::checked_capacity(
                   minimal_capacity, max_capacity)),
               allocator),
      m_mask(m_buffer.size() - 1) {
    static_assert(std::is_default_constructible_v<T>,
                  "T must be default constructible");
}

template <typename T, typename Allocator>
std::size_t queue<T, Allocator>::capacity() const noexcept {
    return m_buffer.size();
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(const T& item) noexcept(
    std::is_nothrow_copy_assignable_v<T>) {
    return try_push_impl(item);
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(T&& item) noexcept(
    std::is_nothrow_move_assignable_v<T>) {
    return try_push_impl(std::move(item));
}

template <typename T, typename Allocator>
template <typename U>
bool queue<T, Allocator>::try_push_impl(U&& item) noexcept(
    std::is_nothrow_assignable_v<T&, U&&>) {
    const auto head = m_head.load(std::memory_order_relaxed);
    // check if full
    if (head - m_saved_tail == m_buffer.size()) {
        m_saved_tail = m_tail.load(std::memory_order_acquire);
        if (head - m_saved_tail == m_buffer.size()) {
            return false;
        }
    }
    m_buffer[head & m_mask] = std::forward<U>(item);
    m_head.store(head + 1, std::memory_order_release);
    return true;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_pop(T& item) noexcept(
    std::is_nothrow_move_assignable_v<T>) {
    const auto tail = m_tail.load(std::memory_order_relaxed);
    // check if empty
    if (m_saved_head == tail) {
        m_saved_head = m_head.load(std::memory_order_acquire);
        if (m_saved_head == tail) {
            return false;
        }
    }
    item = std::move(m_buffer[tail & m_mask]);
    m_tail.store(tail + 1, std::memory_order_release);
    return true;
}
}  // namespace v5
}  // namespace spsc
