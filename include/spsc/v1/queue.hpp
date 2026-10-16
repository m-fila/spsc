#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <mutex>
#include <vector>

namespace spsc {
namespace v1 {
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
    std::mutex m_mtx;
    std::vector<T> m_buffer;
    std::size_t m_head = 0u;
    std::size_t m_tail = 0u;
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

template <typename T>
bool queue<T>::try_pop(T& item) {
    auto lock = std::lock_guard(m_mtx);
    // check if empty
    if (m_head == m_tail) {
        return false;
    }
    item = m_buffer[m_tail];
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
