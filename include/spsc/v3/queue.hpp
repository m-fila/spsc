#pragma once

#include <atomic>
#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "spsc/detail/checked_capacity.hpp"
#include "spsc/detail/wrap_index.hpp"

namespace spsc {
namespace v3 {
/// Single-producer single-consumer bounded queue.
///
/// Changes the memory ordering of v2's atomic index operations to
/// acquire/release.
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

    template <typename U>
    bool try_push_impl(U&& item) noexcept(
        std::is_nothrow_assignable_v<T&, U&&>);
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
    // wrap around if needed
    const auto head = m_head.load(std::memory_order_acquire);
    const auto next_head = detail::wrap_index(head, m_buffer.size());
    // check if full
    if (next_head == m_tail.load(std::memory_order_acquire)) {
        return false;
    }
    m_buffer[head] = std::forward<U>(item);
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
    const auto next_tail = detail::wrap_index(tail, m_buffer.size());
    m_tail.store(next_tail, std::memory_order_release);
    return true;
}
}  // namespace v3
}  // namespace spsc
