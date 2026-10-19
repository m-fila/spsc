#include <gtest/gtest.h>

#include <memory_resource>

#include "counting_memory_resource.hpp"
#include "custom_types.hpp"
#include "spsc/detail/storage.hpp"

TEST(DetailStorage, ConstructsAndDestroysElementsInAllocatedSlots) {
    counting_memory_resource resource;
    using allocator_type =
        std::pmr::polymorphic_allocator<custom_construction_only_type>;
    allocator_type allocator(&resource);
    int destruction_count = 0;

    {
        spsc::detail::storage<custom_construction_only_type, allocator_type>
            slots(2, allocator);

        EXPECT_EQ(slots.capacity(), 2);
        EXPECT_EQ(resource.get_allocations(), 1);

        slots.construct(1, 42, destruction_count);
        EXPECT_EQ(slots[1].value(), 42);  // NOLINT
        EXPECT_EQ(destruction_count, 0);

        slots.destroy(1);
        EXPECT_EQ(destruction_count, 1);
    }

    EXPECT_EQ(destruction_count, 1);
}

TEST(DetailStorage, WorksWithStdAllocator) {
    using allocator_type = std::allocator<custom_construction_only_type>;
    auto allocator = allocator_type{};
    int destruction_count = 0;

    {
        auto storage = spsc::detail::storage<custom_construction_only_type,
                                             allocator_type>(1, allocator);

        storage.construct(0, 42, destruction_count);
        EXPECT_EQ(storage[0].value(), 42);  // NOLINT
        storage.destroy(0);
        EXPECT_EQ(destruction_count, 1);
    }

    EXPECT_EQ(destruction_count, 1);
}
