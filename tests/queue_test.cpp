#include <gtest/gtest.h>

#include <stdexcept>
#include <thread>
#include <type_traits>

#include "spsc/spsc.hpp"

// clang-format off
using QueueTypes = ::testing::Types<
    spsc::v1::queue<int>,
    spsc::v2::queue<int>,
    spsc::v2a::queue<int>,
    spsc::v3::queue<int>,
    spsc::v3a::queue<int>,
    spsc::v4::queue<int>,
    spsc::v4a::queue<int>,
    spsc::v5::queue<int>,
    spsc::v5a::queue<int>
>;
// clang-format on

template <typename T>
class QueueTest : public ::testing::Test {};

TYPED_TEST_SUITE(QueueTest, QueueTypes);

TYPED_TEST(QueueTest, Basics_Capacity) {
    using queue_type = TypeParam;
    {
        auto q = queue_type(1);
        ASSERT_EQ(q.capacity(), 1);
    }
    {
        auto q = queue_type(16);
        ASSERT_EQ(q.capacity(), 16);
    }
    {
        auto q = queue_type(1);
        ASSERT_EQ(q.capacity(), 1);
    }
    {
        auto q = queue_type(16);
        ASSERT_EQ(q.capacity(), 16);
    }
}

TYPED_TEST(QueueTest, ST_TryPopEmpty) {
    using queue_type = TypeParam;
    auto q = queue_type(8);
    auto item = -1;
    ASSERT_FALSE(q.try_pop(item));
    ASSERT_EQ(item, -1);  // item should remain unchanged
}

TYPED_TEST(QueueTest, ST_TryPushAndPop) {
    using queue_type = TypeParam;
    auto q = queue_type(8);
    auto item = -1;

    ASSERT_TRUE(q.try_push(42));
    ASSERT_TRUE(q.try_pop(item));
    ASSERT_EQ(item, 42);
}

TYPED_TEST(QueueTest, ST_TryPushFull) {
    using queue_type = TypeParam;
    auto q = queue_type(2);
    ASSERT_TRUE(q.try_push(1));
    ASSERT_TRUE(q.try_push(2));
    ASSERT_FALSE(q.try_push(3));  // Queue is full
}

TYPED_TEST(QueueTest, ST_TryPopEmptyAfterPush) {
    using queue_type = TypeParam;
    auto q = queue_type(4);
    auto item = -1;

    ASSERT_TRUE(q.try_push(42));
    ASSERT_TRUE(q.try_pop(item));
    ASSERT_EQ(item, 42);

    // Now the queue is empty again
    ASSERT_FALSE(q.try_pop(item));
    ASSERT_EQ(item, 42);  // item should remain unchanged
}

TYPED_TEST(QueueTest, ST_WrapAround) {
    using queue_type = TypeParam;
    {
        auto q = queue_type(4);
        auto item = -1;

        ASSERT_TRUE(q.try_push(1));
        ASSERT_TRUE(q.try_push(2));
        ASSERT_TRUE(q.try_push(3));
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, 1);
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, 2);

        ASSERT_TRUE(q.try_push(4));
        ASSERT_TRUE(q.try_push(5));  // This wraps around the buffer
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, 3);
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, 4);
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, 5);
    }
}

TYPED_TEST(QueueTest, RejectsNonPowerOfTwoCapacity) {
    if constexpr (std::is_same_v<TypeParam, spsc::v5a::queue<int>>) {
        EXPECT_THROW(TypeParam(3), std::invalid_argument);
    }
}

TYPED_TEST(QueueTest, MT_ProducerConsumer) {
    using queue_type = TypeParam;
    auto q = queue_type(128);
    const auto num_items = 1000;

    auto producer = std::thread([&]() {
        for (int i = 0; i < num_items; ++i) {
            while (!q.try_push(i)) {
                // Busy wait until there's space
            }
        }
    });

    auto consumer = std::thread([&]() {
        int item;
        for (int i = 0; i < num_items; ++i) {
            while (!q.try_pop(item)) {
                // Busy wait until there's an item
            }
            ASSERT_EQ(item, i);
        }
    });

    producer.join();
    consumer.join();
}
