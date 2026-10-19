#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

#include "spsc/detail/bit.hpp"
#include "spsc/detail/checked_capacity.hpp"
#include "spsc/detail/storage.hpp"

namespace spsc {
namespace v7 {
/// Single-producer single-consumer bounded queue.
///
/// Based on v5 but replaced the vector with custom storage to support
/// non-default-constructible types.
/// Do not destroy the queue while another thread is accessing it.
template <typename T, typename Allocator = std::allocator<T>>
class queue {
    static_assert(std::is_object_v<T> && std::is_same_v<T, std::remove_cv_t<T>>,
                  "T must be a cv-unqualified object type");

    public:
    using value_type = T;
    using allocator_type = Allocator;
    static constexpr std::size_t max_capacity =
        detail::max_power2<std::size_t>();

    /// Creates an empty queue with capacity rounded up to a power of two.
    explicit queue(std::size_t capacity,
                   const Allocator& allocator = Allocator());
    queue(const queue&) = delete;
    queue(queue&&) = delete;
    queue& operator=(const queue&) = delete;
    queue& operator=(queue&&) = delete;
    /// Destroys the queued elements and releases the storage.
    ~queue();

    /// Returns the usable queue capacity.
    std::size_t capacity() const noexcept;
    /// Constructs an item at the back; returns false if full.
    /// Requires T constructible from the supplied arguments.
    template <typename... Args>
    bool emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args&&...>);
    /// Copies an item to the back; returns false if full.
    /// Requires copy-constructible T.
    bool try_push(const T& item) noexcept(
        std::is_nothrow_constructible_v<T, const T&>);
    /// Moves an item to the back; returns false if full.
    /// Requires move-constructible T.
    bool try_push(T&& item) noexcept(std::is_nothrow_constructible_v<T, T&&>);
    /// (Re)moves and returns the front item, or an empty optional if empty.
    /// Requires move-constructible T.
    std::optional<T> try_pop() noexcept(
        std::is_nothrow_move_constructible_v<T>);
    /// Moves the front item into item; returns false if empty.
    /// Requires move-assignable T.
    bool try_pop(T& item) noexcept(std::is_nothrow_move_assignable_v<T>);

    private:
    detail::storage<T, Allocator> m_buffer;
    const std::size_t m_mask;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_head = 0u;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_tail = 0u;
    alignas(std::hardware_destructive_interference_size) std::size_t
        m_saved_head = 0u;
    alignas(std::hardware_destructive_interference_size) std::size_t
        m_saved_tail = 0u;

    template <typename Output>
    bool try_pop_impl(Output& output) noexcept(
        (std::is_same_v<Output, std::optional<T>>
             ? std::is_nothrow_move_constructible_v<T>
             : std::is_nothrow_move_assignable_v<T>));
};

template <typename T, typename Allocator>
queue<T, Allocator>::queue(std::size_t minimal_capacity,
                           const Allocator& allocator)
    : m_buffer(spsc::detail::next_power2(spsc::detail::checked_capacity(
                   minimal_capacity, max_capacity)),
               allocator),
      m_mask(m_buffer.capacity() - 1) {}

template <typename T, typename Allocator>
queue<T, Allocator>::~queue() {
    auto tail = m_tail.load(std::memory_order_relaxed);
    const auto head = m_head.load(std::memory_order_relaxed);
    while (tail != head) {
        m_buffer.destroy(tail & m_mask);
        ++tail;
    }
}

template <typename T, typename Allocator>
std::size_t queue<T, Allocator>::capacity() const noexcept {
    return m_buffer.capacity();
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(const T& item) noexcept(
    std::is_nothrow_constructible_v<T, const T&>) {
    return emplace(item);
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(T&& item) noexcept(
    std::is_nothrow_constructible_v<T, T&&>) {
    return emplace(std::move(item));
}

template <typename T, typename Allocator>
template <typename... Args>
bool queue<T, Allocator>::emplace(Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, Args&&...>) {
    const auto head = m_head.load(std::memory_order_relaxed);
    // check if full
    if (head - m_saved_tail == m_buffer.capacity()) {
        m_saved_tail = m_tail.load(std::memory_order_acquire);
        if (head - m_saved_tail == m_buffer.capacity()) {
            return false;
        }
    }
    m_buffer.construct(head & m_mask, std::forward<Args>(args)...);
    m_head.store(head + 1, std::memory_order_release);
    return true;
}

template <typename T, typename Allocator>
std::optional<T> queue<T, Allocator>::try_pop() noexcept(
    std::is_nothrow_move_constructible_v<T>) {
    std::optional<T> item;
    if (!try_pop_impl(item)) {
        return std::nullopt;
    }
    return item;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_pop(T& item) noexcept(
    std::is_nothrow_move_assignable_v<T>) {
    return try_pop_impl(item);
}

template <typename T, typename Allocator>
template <typename Output>
bool queue<T, Allocator>::try_pop_impl(Output& output) noexcept(
    (std::is_same_v<Output, std::optional<T>>
         ? std::is_nothrow_move_constructible_v<T>
         : std::is_nothrow_move_assignable_v<T>)) {
    const auto tail = m_tail.load(std::memory_order_relaxed);
    // check if empty
    if (m_saved_head == tail) {
        m_saved_head = m_head.load(std::memory_order_acquire);
        if (m_saved_head == tail) {
            return false;
        }
    }
    const auto index = tail & m_mask;
    if constexpr (std::is_same_v<Output, std::optional<T>>) {
        output.emplace(std::move(m_buffer[index]));
    } else {
        output = std::move(m_buffer[index]);
    }
    m_buffer.destroy(index);
    m_tail.store(tail + 1, std::memory_order_release);
    return true;
}
}  // namespace v7
}  // namespace spsc
