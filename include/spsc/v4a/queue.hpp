#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

namespace spsc {
namespace v4a {
template <typename T>

class queue {
    public:
    using value_type = T;
    static constexpr std::size_t max_capacity =
        std::numeric_limits<std::size_t>::max() > 0
            ? std::numeric_limits<std::size_t>::max() - 1
            : 0;

    explicit queue(std::size_t capacity);

    std::size_t capacity() const;
    bool try_push(const T& item);
    bool try_pop(T& item);

    private:
    std::vector<T> m_buffer;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_head = 0u;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_tail = 0u;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_saved_head = 0u;
    alignas(std::hardware_destructive_interference_size)
        std::atomic<std::size_t> m_saved_tail = 0u;
};

template <typename T>
queue<T>::queue(std::size_t capacity) : m_buffer(capacity + 1) {
    assert(capacity > 0);
    assert(capacity <= max_capacity);
    assert(capacity + 1 <= m_buffer.max_size());
}

template <typename T>
std::size_t queue<T>::capacity() const {
    return m_buffer.size() - 1;
}

template <typename T>
bool queue<T>::try_push(const T& item) {
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

template <typename T>
bool queue<T>::try_pop(T& item) {
    const auto tail = m_tail.load(std::memory_order_relaxed);
    // check if empty
    if (m_saved_head == tail) {
        m_saved_head = m_head.load(std::memory_order_acquire);
        if (m_saved_head == tail) {
            return false;
        }
    }
    item = m_buffer[tail];
    // wrap around if needed
    auto next_tail = tail + 1;
    if (next_tail == m_buffer.size()) {
        next_tail = 0;
    }
    m_tail.store(next_tail, std::memory_order_release);
    return true;
}
}  // namespace v4a
}  // namespace spsc
