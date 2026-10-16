#include <gtest/gtest.h>

#include <stdexcept>
#include <thread>
#include <type_traits>

#include "counting_memory_resource.hpp"
#include "custom_types.hpp"
#include "spsc/spsc.hpp"

// clang-format off
#define QUEUE_TYPES(T)  \
    spsc::v1::queue<T>, \
    spsc::v2::queue<T>, \
    spsc::v3::queue<T>, \
    spsc::v4::queue<T>, \
    spsc::v5::queue<T>, \
    spsc::v6::queue<T>

using QueueTypes = ::testing::Types<
    QUEUE_TYPES(int),
    QUEUE_TYPES(custom_move_only_type),
    QUEUE_TYPES(custom_copy_only_type)
>;

#undef QUEUE_TYPES
// clang-format on

template <typename T>
class QueueTest : public ::testing::Test {};

TYPED_TEST_SUITE(QueueTest, QueueTypes);

TYPED_TEST(QueueTest, Basics_Capacity) {
    using queue_type = TypeParam;
    for (auto capacity :
         {1, 2, 3, 4, 5, 8, 11, 12, 16, 23, 24, 25, 32, 64, 128}) {
        auto q = queue_type(capacity);
        ASSERT_GE(q.capacity(), capacity);
    }
}

TYPED_TEST(QueueTest, ST_TryPopEmpty) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(8);
    auto item = item_type{-1};
    ASSERT_FALSE(q.try_pop(item));
    ASSERT_EQ(item, -1);  // item should remain unchanged
}

TYPED_TEST(QueueTest, ST_TryPushAndPop) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(8);
    auto item = item_type{-1};

    ASSERT_TRUE(q.try_push(item_type{42}));
    ASSERT_TRUE(q.try_pop(item));
    ASSERT_EQ(item, item_type{42});
}

TYPED_TEST(QueueTest, ST_TryPushFull) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(2);
    ASSERT_TRUE(q.try_push(item_type{1}));
    ASSERT_TRUE(q.try_push(item_type{2}));
    ASSERT_FALSE(q.try_push(item_type{3}));  // Queue is full
}

TYPED_TEST(QueueTest, ST_TryPopEmptyAfterPush) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(4);
    auto item = item_type{-1};

    ASSERT_TRUE(q.try_push(item_type{42}));
    ASSERT_TRUE(q.try_pop(item));
    ASSERT_EQ(item, item_type{42});

    // Now the queue is empty again
    ASSERT_FALSE(q.try_pop(item));
    ASSERT_EQ(item, item_type{42});  // item should remain unchanged
}

TYPED_TEST(QueueTest, ST_WrapAround) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    {
        auto q = queue_type(4);
        auto item = item_type{-1};

        ASSERT_TRUE(q.try_push(item_type{1}));
        ASSERT_TRUE(q.try_push(item_type{2}));
        ASSERT_TRUE(q.try_push(item_type{3}));
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{1});
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{2});

        ASSERT_TRUE(q.try_push(item_type{4}));
        ASSERT_TRUE(q.try_push(item_type{5}));  // This wraps around the buffer
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{3});
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{4});
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{5});
    }
    {
        auto q = queue_type(4);
        auto item = item_type{-1};

        ASSERT_TRUE(q.try_push(item_type{1}));
        ASSERT_TRUE(q.try_push(item_type{2}));
        ASSERT_TRUE(q.try_push(item_type{3}));
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{1});
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{2});

        ASSERT_TRUE(q.try_push(item_type{4}));
        ASSERT_TRUE(q.try_push(item_type{5}));  // This wraps around the buffer
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{3});
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{4});
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{5});
    }
}

TYPED_TEST(QueueTest, MT_ProducerConsumer) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(126);
    const auto num_items = 1000;

    auto producer = std::thread([&]() {
        for (int i = 0; i < num_items; ++i) {
            while (!q.try_push(item_type{i})) {
                // Busy wait until there's space
            }
        }
    });

    auto consumer = std::thread([&]() {
        auto item = item_type{-1};
        for (int i = 0; i < num_items; ++i) {
            while (!q.try_pop(item)) {
                // Busy wait until there's an item
            }
            ASSERT_EQ(item, item_type{i});
        }
    });

    producer.join();
    consumer.join();
}

// clang-format off
using AllocatorQueueTypes = ::testing::Types<
    spsc::v1::queue<int, std::pmr::polymorphic_allocator<int>>,
    spsc::v2::queue<int, std::pmr::polymorphic_allocator<int>>,
    spsc::v3::queue<int, std::pmr::polymorphic_allocator<int>>,
    spsc::v4::queue<int, std::pmr::polymorphic_allocator<int>>,
    spsc::v5::queue<int, std::pmr::polymorphic_allocator<int>>,
    spsc::v6::queue<int, std::pmr::polymorphic_allocator<int>>
>;
// clang-format on

template <typename T>
class QueueAllocatorTest : public ::testing::Test {};

TYPED_TEST_SUITE(QueueAllocatorTest, AllocatorQueueTypes);

TYPED_TEST(QueueAllocatorTest, UsesProvidedAllocator) {
    using queue_type = TypeParam;
    auto resource = counting_memory_resource();
    ASSERT_EQ(resource.allocations, 0);
    auto allocator = std::pmr::polymorphic_allocator<int>(&resource);
    auto q = queue_type(8, allocator);
    ASSERT_EQ(resource.allocations, 1);
}

TYPED_TEST(QueueAllocatorTest, NoExtraAllocations) {
    using queue_type = TypeParam;
    auto resource = counting_memory_resource();
    ASSERT_EQ(resource.allocations, 0);
    auto q = queue_type(8, &resource);
    ASSERT_EQ(resource.allocations, 1);
    // fully fill the queue
    while (!q.try_push(42)) {
    }
    ASSERT_EQ(resource.allocations, 1);
    auto item = typename queue_type::value_type{-1};
    EXPECT_TRUE(q.try_pop(item));
    EXPECT_EQ(item, 42);
    ASSERT_EQ(resource.allocations, 1);
    EXPECT_TRUE(q.try_push(43));
    ASSERT_EQ(resource.allocations, 1);
}
