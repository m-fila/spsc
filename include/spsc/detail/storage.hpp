#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace spsc::detail {
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
/// Owns raw allocator-backed storage with explicitly managed element lifetimes.
template <typename T, typename Allocator>
class storage {
    using allocator_traits = std::allocator_traits<Allocator>;

    static_assert(std::is_same_v<typename allocator_traits::pointer, T*>,
                  "Allocator must use T* as its pointer type");

    public:
    explicit storage(std::size_t capacity, const Allocator& allocator)
        : m_allocator(allocator),
          m_data(allocator_traits::allocate(m_allocator, capacity)),
          m_capacity(capacity) {}

    storage(const storage&) = delete;
    storage(storage&&) = delete;
    storage& operator=(const storage&) = delete;
    storage& operator=(storage&&) = delete;

    ~storage() {
        allocator_traits::deallocate(m_allocator, m_data, m_capacity);
    }

    std::size_t capacity() const noexcept { return m_capacity; }

    T& operator[](std::size_t index) noexcept { return m_data[index]; }

    template <typename... Args>
    void construct(std::size_t index, Args&&... args) noexcept(
        noexcept(allocator_traits::construct(std::declval<Allocator&>(),
                                             std::declval<T*>(),
                                             std::declval<Args&&>()...))) {
        allocator_traits::construct(m_allocator, m_data + index,
                                    std::forward<Args>(args)...);
    }

    void destroy(std::size_t index) noexcept(noexcept(allocator_traits::destroy(
        std::declval<Allocator&>(), std::declval<T*>()))) {
        allocator_traits::destroy(m_allocator, m_data + index);
    }

    private:
    Allocator m_allocator;
    T* m_data;
    const std::size_t m_capacity;
};
// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

}  // namespace spsc::detail
