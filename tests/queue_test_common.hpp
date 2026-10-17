#include <gtest/gtest.h>

#include <thread>

#include "counting_memory_resource.hpp"
#include "custom_types.hpp"

template <typename T>
class QueueTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(QueueTest);
TYPED_TEST_P(QueueTest, Basics_Capacity) {
    using queue_type = TypeParam;
    for (auto capacity :
         {1, 2, 3, 4, 5, 8, 11, 12, 16, 23, 24, 25, 32, 64, 128}) {
        auto q = queue_type(capacity);
        ASSERT_GE(q.capacity(), capacity);
    }
}

TYPED_TEST_P(QueueTest, ST_TryPopEmpty) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(8);
    auto item = item_type{-1};
    ASSERT_FALSE(q.try_pop(item));
    ASSERT_EQ(item, -1);  // item should remain unchanged
}

TYPED_TEST_P(QueueTest, ST_TryPushAndPop) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(8);
    auto item = item_type{-1};

    ASSERT_TRUE(q.try_push(item_type{42}));
    ASSERT_TRUE(q.try_pop(item));
    ASSERT_EQ(item, item_type{42});
}

TYPED_TEST_P(QueueTest, ST_TryPushFull) {
    GTEST_SKIP();
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(2);
    ASSERT_TRUE(q.try_push(item_type{1}));
    ASSERT_TRUE(q.try_push(item_type{2}));
    ASSERT_FALSE(q.try_push(item_type{3}));  // Queue is full
}

TYPED_TEST_P(QueueTest, ST_TryPopEmptyAfterPush) {
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

TYPED_TEST_P(QueueTest, ST_WrapAround) {
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

TYPED_TEST_P(QueueTest, MT_ProducerConsumer) {
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

REGISTER_TYPED_TEST_SUITE_P(QueueTest, Basics_Capacity, ST_TryPopEmpty,
                            ST_TryPushAndPop, ST_TryPushFull,
                            ST_TryPopEmptyAfterPush, ST_WrapAround,
                            MT_ProducerConsumer);

template <typename T>
class QueueAllocatorTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(QueueAllocatorTest);

TYPED_TEST_P(QueueAllocatorTest, UsesProvidedAllocator) {
    using queue_type = TypeParam;
    auto resource = counting_memory_resource();
    ASSERT_EQ(resource.allocations, 0);
    auto allocator = std::pmr::polymorphic_allocator<int>(&resource);
    auto q = queue_type(8, allocator);
    ASSERT_EQ(resource.allocations, 1);
}

TYPED_TEST_P(QueueAllocatorTest, NoExtraAllocations) {
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

REGISTER_TYPED_TEST_SUITE_P(QueueAllocatorTest, UsesProvidedAllocator,
                            NoExtraAllocations);

#define SPSC_INSTANTIATE_QUEUE_TESTS(queue_template, prefix)              \
    using QueueTypes =                                                    \
        ::testing::Types<queue_template<int>,                             \
                         queue_template<custom_move_only_type>,           \
                         queue_template<custom_copy_only_type>>;          \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix, QueueTest, QueueTypes);        \
    using AllocatorQueueTypes = ::testing::Types<                         \
        queue_template<int, std::pmr::polymorphic_allocator<int>>>;       \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix##Allocator, QueueAllocatorTest, \
                                   AllocatorQueueTypes)
