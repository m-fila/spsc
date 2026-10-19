#pragma once

#include <atomic>
#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "spsc/detail/checked_capacity.hpp"
#include "spsc/detail/iterate_and_wrap.hpp"

namespace spsc {
namespace v3 {
/// Single-producer single-consumer bounded queue.
///
/// Changes the memory ordering of v2's atomic index operations to
/// acquire/release.
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
    bool try_push(const T& item) noexcept(std::is_nothrow_copy_assignable_v<T>);
    /// Moves an item to the back; returns false if full.
    /// Requires move-assignable T.
    bool try_push(T&& item) noexcept(std::is_nothrow_move_assignable_v<T>);
    /// Moves the front item into item; returns false if empty.
    /// Requires move-assignable T.
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
    const auto next_head = detail::iterate_and_wrap(head, m_buffer.size());
    // check if full
    if (next_head == m_tail.load(std::memory_order_acquire)) {
        return false;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    item = std::move(m_buffer[tail]);
    // wrap around if needed
    const auto next_tail = detail::iterate_and_wrap(tail, m_buffer.size());
    m_tail.store(next_tail, std::memory_order_release);
    return true;
}
}  // namespace v3
}  // namespace spsc
