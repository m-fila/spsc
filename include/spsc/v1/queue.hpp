#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace spsc {
namespace v1 {
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
    // mutex-based implementation can throw on lock
    bool try_push(const T& item);
    // mutex-based implementation can throw on lock
    bool try_push(T&& item);
    // mutex-based implementation can throw on lock
    bool try_pop(T& item);

    private:
    std::mutex m_mtx;
    std::vector<T, Allocator> m_buffer;
    std::size_t m_head = 0u;
    std::size_t m_tail = 0u;
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
bool queue<T, Allocator>::try_push(const T& item) {
    auto lock = std::lock_guard(m_mtx);
    // wrap around if needed
    auto next_head = m_head + 1;
    if (next_head == m_buffer.size()) {
        next_head = 0;
    }
    // check if full
    if (next_head == m_tail) {
        return false;
    }
    m_buffer[m_head] = item;
    m_head = next_head;
    return true;
}

template <typename T, typename Allocator>
bool queue<T, Allocator>::try_push(T&& item) {
    auto lock = std::lock_guard(m_mtx);
    // wrap around if needed
    auto next_head = m_head + 1;
    if (next_head == m_buffer.size()) {
        next_head = 0;
    }
    // check if full
    if (next_head == m_tail) {
        return false;
    }
    if constexpr (std::is_move_assignable_v<T>) {
        m_buffer[m_head] = std::move(item);
    } else {
        m_buffer[m_head] = item;
    }
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
    if constexpr (std::is_move_assignable_v<T>) {
        item = std::move(m_buffer[m_tail]);
    } else {
        item = m_buffer[m_tail];
    }
    // wrap around if needed
    auto next_tail = m_tail + 1;
    if (next_tail == m_buffer.size()) {
        next_tail = 0;
    }
    m_tail = next_tail;
    return true;
}
}  // namespace v1
}  // namespace spsc
