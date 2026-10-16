#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace spsc {
namespace v4 {
template <typename T, typename Allocator = std::allocator<T>>

class queue {
    public:
    using value_type = T;
    using allocator_type = Allocator;
    static constexpr std::size_t max_capacity =
        std::numeric_limits<std::size_t>::max() > 0
            ? std::numeric_limits<std::size_t>::max() - 1
            : 0;

    explicit queue(std::size_t capacity,
                   const Allocator& allocator = Allocator());

    std::size_t capacity() const noexcept;
    bool try_push(const T& item) noexcept(std::is_nothrow_copy_assignable_v<T>);
    bool try_push(T&& item) noexcept(std::is_nothrow_move_assignable_v<T> ||
                                     std::is_nothrow_copy_assignable_v<T>);
    bool try_pop(T& item) noexcept(std::is_nothrow_move_assignable_v<T> ||
                                   std::is_nothrow_copy_assignable_v<T>);

    private:
    std::vector<T, Allocator> m_buffer;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_head = 0u;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_tail = 0u;
    alignas(std::hardware_destructive_interference_size) std::size_t
        m_saved_head = 0u;
    alignas(std::hardware_destructive_interference_size) std::size_t
        m_saved_tail = 0u;
};

template <typename T, typename Allocator>
queue<T, Allocator>::queue(std::size_t capacity, const Allocator& allocator)
    : m_buffer(capacity + 1, allocator) {
    assert(capacity > 0);
    assert(capacity <= max_capacity);
    assert(capacity + 1 <= m_buffer.max_size());
    static_assert(std::is_default_constructible_v<T>,
                  "T must be default constructible");
    // overeager check since in principle it could be possible to just construct
    // the queue but never push/pop anything
    static_assert(std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>,
                  "T must be move or copy assignable");
}

template <typename T, typename Allocator>
std::size_t queue<T, Allocator>::capacity() const noexcept {
    return m_buffer.size() - 1;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(const T& item) noexcept(
    std::is_nothrow_copy_assignable_v<T>) {
    // wrap around if needed
    const auto head = m_head.load(std::memory_order_relaxed);
    auto next_head = head + 1;
    if (next_head == m_buffer.size()) {
        next_head = 0;
    }
    // check if full
    if (next_head == m_saved_tail) {
        m_saved_tail = m_tail.load(std::memory_order_acquire);
        if (next_head == m_saved_tail) {
            return false;
        }
    }
    m_buffer[head] = item;
    m_head.store(next_head, std::memory_order_release);
    return true;
}
template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(T&& item) noexcept(
    std::is_nothrow_move_assignable_v<T> ||
    std::is_nothrow_copy_assignable_v<T>) {
    // wrap around if needed
    const auto head = m_head.load(std::memory_order_relaxed);
    auto next_head = head + 1;
    if (next_head == m_buffer.size()) {
        next_head = 0;
    }
    // check if full
    if (next_head == m_saved_tail) {
        m_saved_tail = m_tail.load(std::memory_order_acquire);
        if (next_head == m_saved_tail) {
            return false;
        }
    }
    if constexpr (std::is_nothrow_move_assignable_v<T>) {
        m_buffer[head] = std::move(item);
    } else if constexpr (std::is_nothrow_copy_assignable_v<T>) {
        m_buffer[head] = item;
    } else if constexpr (std::is_move_assignable_v<T>) {
        m_buffer[head] = std::move(item);
    } else {
        m_buffer[head] = item;
    }
    m_head.store(next_head, std::memory_order_release);
    return true;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_pop(T& item) noexcept(
    std::is_nothrow_move_assignable_v<T> ||
    std::is_nothrow_copy_assignable_v<T>) {
    const auto tail = m_tail.load(std::memory_order_relaxed);
    // check if empty
    if (m_saved_head == tail) {
        m_saved_head = m_head.load(std::memory_order_acquire);
        if (m_saved_head == tail) {
            return false;
        }
    }
    if constexpr (std::is_nothrow_move_assignable_v<T>) {
        item = std::move(m_buffer[tail]);
    } else if constexpr (std::is_nothrow_copy_assignable_v<T>) {
        item = m_buffer[tail];
    } else if constexpr (std::is_move_assignable_v<T>) {
        item = std::move(m_buffer[tail]);
    } else {
        item = m_buffer[tail];
    }
    // wrap around if needed
    auto next_tail = tail + 1;
    if (next_tail == m_buffer.size()) {
        next_tail = 0;
    }
    m_tail.store(next_tail, std::memory_order_release);
    return true;
}
}  // namespace v4
}  // namespace spsc
