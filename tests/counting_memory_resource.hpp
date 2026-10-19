#pragma once
#include <cstddef>
#include <memory_resource>

class counting_memory_resource : public std::pmr::memory_resource {
    public:
    explicit counting_memory_resource(
        std::pmr::memory_resource* upstream =
            std::pmr::new_delete_resource()) noexcept
        : m_upstream(upstream) {}

    std::size_t get_allocations() const noexcept { return allocations; }

    private:
    std::size_t allocations = 0;
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        auto* pointer = m_upstream->allocate(bytes, alignment);
        ++allocations;
        return pointer;
    }

    void do_deallocate(void* pointer, std::size_t bytes,
                       std::size_t alignment) override {
        m_upstream->deallocate(pointer, bytes, alignment);
    }

    bool do_is_equal(
        const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    std::pmr::memory_resource* m_upstream;
};
