#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace spsc {
namespace v5a {
template <typename T>

class queue {
    public:
    using value_type = T;
    static constexpr std::size_t max_capacity =
        std::size_t{1} << (std::numeric_limits<std::size_t>::digits - 1);

    explicit queue(std::size_t capacity);

    std::size_t capacity() const;
    bool try_push(const T& item);
    bool try_pop(T& item);

    private:
    static std::size_t checked_capacity(std::size_t capacity);

    std::vector<T> m_buffer;
    const std::size_t m_mask;
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
std::size_t queue<T>::checked_capacity(std::size_t capacity) {
    if (capacity == 0 || (capacity & (capacity - 1)) != 0) {
        throw std::invalid_argument("queue capacity must be a power of two");
    }
    if (capacity > max_capacity) {
        throw std::length_error("queue capacity is too large");
    }
    return capacity;
}

template <typename T>
queue<T>::queue(std::size_t capacity)
    : m_buffer(checked_capacity(capacity)), m_mask(capacity - 1) {}

template <typename T>
std::size_t queue<T>::capacity() const {
    return m_buffer.size();
}

template <typename T>
bool queue<T>::try_push(const T& item) {
    const auto head = m_head.load(std::memory_order_relaxed);
    auto saved_tail = m_saved_tail.load(std::memory_order_relaxed);
    if (head - saved_tail == m_buffer.size()) {
        saved_tail = m_tail.load(std::memory_order_acquire);
        m_saved_tail.store(saved_tail, std::memory_order_relaxed);
        if (head - saved_tail == m_buffer.size()) {
            return false;
        }
    }
    m_buffer[head & m_mask] = item;
    m_head.store(head + 1, std::memory_order_release);
    return true;
}

template <typename T>
bool queue<T>::try_pop(T& item) {
    const auto tail = m_tail.load(std::memory_order_relaxed);
    auto saved_head = m_saved_head.load(std::memory_order_relaxed);
    if (saved_head == tail) {
        saved_head = m_head.load(std::memory_order_acquire);
        m_saved_head.store(saved_head, std::memory_order_relaxed);
        if (saved_head == tail) {
            return false;
        }
    }
    item = m_buffer[tail & m_mask];
    m_tail.store(tail + 1, std::memory_order_release);
    return true;
}
}  // namespace v5a
}  // namespace spsc
