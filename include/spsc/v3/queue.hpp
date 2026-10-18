#pragma once

#include <atomic>
#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "spsc/detail/checked_capacity.hpp"

namespace spsc {
namespace v3 {
template <typename T, typename Allocator = std::allocator<T>>

class queue {
    public:
    using value_type = T;
    using allocator_type = Allocator;
    static constexpr std::size_t max_capacity =
        std::numeric_limits<std::size_t>::max() - 1;

    explicit queue(std::size_t capacity,
                   const Allocator& allocator = Allocator());

    std::size_t capacity() const noexcept;
    bool try_push(const T& item) noexcept(std::is_nothrow_copy_assignable_v<T>);
    bool try_push(T&& item) noexcept(std::is_nothrow_move_assignable_v<T>);
    bool try_pop(T& item) noexcept(std::is_nothrow_move_assignable_v<T>);

    private:
    std::vector<T, Allocator> m_buffer;
    std::atomic<std::size_t> m_head = 0u;
    std::atomic<std::size_t> m_tail = 0u;
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
bool queue<T, Allocator>::try_push(const T& item) noexcept(
    std::is_nothrow_copy_assignable_v<T>) {
    // wrap around if needed
    const auto head = m_head.load(std::memory_order_acquire);
    auto next_head = head + 1;
    if (next_head == m_buffer.size()) {
        next_head = 0;
    }
    // check if full
    if (next_head == m_tail.load(std::memory_order_acquire)) {
        return false;
    }
    m_buffer[head] = item;
    m_head.store(next_head, std::memory_order_release);
    return true;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(T&& item) noexcept(
    std::is_nothrow_move_assignable_v<T>) {
    // wrap around if needed
    const auto head = m_head.load(std::memory_order_acquire);
    auto next_head = head + 1;
    if (next_head == m_buffer.size()) {
        next_head = 0;
    }
    // check if full
    if (next_head == m_tail.load(std::memory_order_acquire)) {
        return false;
    }
    m_buffer[head] = std::move(item);
    m_head.store(next_head, std::memory_order_release);
    return true;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_pop(T& item) noexcept(
    std::is_nothrow_move_assignable_v<T>) {
    const auto tail = m_tail.load(std::memory_order_acquire);
    // check if empty
    if (m_head.load(std::memory_order_acquire) == tail) {
        return false;
    }
    item = std::move(m_buffer[tail]);
    // wrap around if needed
    auto next_tail = tail + 1;
    if (next_tail == m_buffer.size()) {
        next_tail = 0;
    }
    m_tail.store(next_tail, std::memory_order_release);
    return true;
}
}  // namespace v3
}  // namespace spsc
