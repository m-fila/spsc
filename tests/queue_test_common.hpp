#include <gtest/gtest.h>

#include <thread>
#include <type_traits>

#include "counting_memory_resource.hpp"
#include "custom_types.hpp"

template <typename T>
class QueueTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(QueueTest);
TYPED_TEST_P(QueueTest, Capacity) {
    using queue_type = TypeParam;
    for (auto capacity :
         {1, 2, 3, 4, 5, 8, 11, 12, 16, 23, 24, 25, 32, 64, 128}) {
        auto q = queue_type(capacity);
        EXPECT_GE(q.capacity(), capacity);
        EXPECT_LE(q.capacity(), queue_type::max_capacity);
    }
}

TYPED_TEST_P(QueueTest, ST_TryPopEmpty) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(8);
    auto item = item_type{42};
    ASSERT_FALSE(q.try_pop(item));
    ASSERT_EQ(item, 42);  // item should remain unchanged
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

TYPED_TEST_P(QueueTest, ST_TryPushAndPopLvalueRef) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    if constexpr (std::is_copy_assignable_v<item_type>) {
        auto q = queue_type(8);
        auto in_item = item_type{-1};
        auto out_item = item_type{-2};

        ASSERT_TRUE(q.try_push(in_item));
        ASSERT_EQ(in_item, item_type{-1});  // in_item should remain unchanged
        ASSERT_TRUE(q.try_pop(out_item));
        ASSERT_EQ(out_item, in_item);  // out_item should be equal to in_item
    }
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

TYPED_TEST_P(QueueTest, ST_TryPushFull) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(7);
    for (std::size_t i = 0; i < q.capacity(); ++i) {
        ASSERT_TRUE(q.try_push(item_type{static_cast<int>(i)}));
    }
    ASSERT_FALSE(q.try_push(item_type{-1}));  // Queue is full
}

TYPED_TEST_P(QueueTest, ST_FillThenEmpty) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(7);
    auto item = item_type{-1};
    // fill the queue to capacity
    for (std::size_t i = 0; i < q.capacity(); ++i) {
        ASSERT_TRUE(q.try_push(item_type{static_cast<int>(i)}));
    }
    ASSERT_FALSE(q.try_push(item_type{-1}));  // Queue is full
    // pop all items
    for (std::size_t i = 0; i < q.capacity(); ++i) {
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{static_cast<int>(i)});
    }
    ASSERT_FALSE(q.try_pop(item));  // Queue is empty
}

TYPED_TEST_P(QueueTest, ST_WrapAround) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(7);
    auto item = item_type{-1};
    // fill the queue to capacity
    for (std::size_t i = 0; i < q.capacity(); ++i) {
        ASSERT_TRUE(q.try_push(item_type{static_cast<int>(i)}));
    }
    ASSERT_FALSE(q.try_push(item_type{-1}));  // Queue is full
    // pop half the times
    for (std::size_t i = 0; i < q.capacity() / 2; ++i) {
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{static_cast<int>(i)});
    }
    // fill the queue to capacity again
    for (std::size_t i = q.capacity(); i < q.capacity() + q.capacity() / 2;
         ++i) {
        ASSERT_TRUE(q.try_push(item_type{static_cast<int>(i)}));
    }
    ASSERT_FALSE(q.try_push(item_type{-1}));  // Queue is full
}

TYPED_TEST_P(QueueTest, ST_WrapAroundLvalueRef) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    if constexpr (std::is_copy_assignable_v<item_type>) {
        auto q = queue_type(7);
        auto out_item = item_type{-1};
        auto some_item = item_type{-2};
        // fill the queue to capacity
        for (std::size_t i = 0; i < q.capacity(); ++i) {
            auto in_item = item_type{static_cast<int>(i)};
            ASSERT_TRUE(q.try_push(in_item));
        }
        ASSERT_FALSE(q.try_push(some_item));  // Queue is full
        // pop half the times
        for (std::size_t i = 0; i < q.capacity() / 2; ++i) {
            ASSERT_TRUE(q.try_pop(out_item));
            ASSERT_EQ(out_item, item_type{static_cast<int>(i)});
        }
        // fill the queue to capacity again
        for (std::size_t i = q.capacity(); i < q.capacity() + q.capacity() / 2;
             ++i) {
            auto in_item = item_type{static_cast<int>(i)};
            ASSERT_TRUE(q.try_push(in_item));
        }
        ASSERT_FALSE(q.try_push(some_item));  // Queue is full
    }
}

TYPED_TEST_P(QueueTest, ST_PushPopCycle) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    auto q = queue_type(3);
    auto item = item_type{-1};

    // Keep pushing and popping
    for (std::size_t i = 0; i < 5 * q.capacity(); ++i) {
        ASSERT_TRUE(q.try_push(item_type{static_cast<int>(i)}));
        ASSERT_TRUE(q.try_pop(item));
        ASSERT_EQ(item, item_type{static_cast<int>(i)});
    }
}

TYPED_TEST_P(QueueTest, ST_PushPopCycleLvalueRef) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    if constexpr (std::is_copy_assignable_v<item_type>) {
        auto q = queue_type(3);
        auto out_item = item_type{-2};

        // Keep pushing and popping
        for (std::size_t i = 0; i < 5 * q.capacity(); ++i) {
            auto in_item = item_type{static_cast<int>(i)};
            ASSERT_TRUE(q.try_push(in_item));
            ASSERT_TRUE(q.try_pop(out_item));
            ASSERT_EQ(out_item, in_item);
        }
    }
}

template <bool use_lvalue_push, typename Queue>
void run_producer_consumer(Queue& queue, int num_items) {
    using item_type = typename Queue::value_type;

    auto producer = std::thread([&]() {
        for (int item_index = 0; item_index < num_items; ++item_index) {
            if constexpr (use_lvalue_push) {
                auto input = item_type{item_index};
                while (!queue.try_push(input)) {
                    std::this_thread::yield();
                }
            } else {
                while (!queue.try_push(item_type{item_index})) {
                    std::this_thread::yield();
                }
            }
        }
    });

    auto consumer = std::thread([&]() {
        auto output = item_type{-1};
        for (int item_index = 0; item_index < num_items; ++item_index) {
            while (!queue.try_pop(output)) {
                std::this_thread::yield();
            }
            EXPECT_EQ(output, item_type{item_index});
        }
    });

    producer.join();
    consumer.join();
}

TYPED_TEST_P(QueueTest, MT_ProducerConsumer) {
    using queue_type = TypeParam;
    auto queue = queue_type(126);
    run_producer_consumer<false>(queue, 1000);
}

TYPED_TEST_P(QueueTest, MT_ProducerConsumerLvalueRef) {
    using queue_type = TypeParam;
    if constexpr (std::is_copy_assignable_v<typename TypeParam::value_type>) {
        auto queue = queue_type(126);
        run_producer_consumer<true>(queue, 1000);
    }
}

TYPED_TEST_P(QueueTest, MT_ProducerConsumerStress) {
    using queue_type = TypeParam;
    auto queue = queue_type(126);
    run_producer_consumer<false>(queue, 1'000'000);
}

TYPED_TEST_P(QueueTest, MT_ProducerConsumerStressLvalueRef) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    if constexpr (std::is_copy_assignable_v<item_type>) {
        auto queue = queue_type(126);
        run_producer_consumer<true>(queue, 1'000'000);
    }
}

TYPED_TEST_P(QueueTest, MT_ProducerConsumerSmallCapacity) {
    using queue_type = TypeParam;
    auto queue = queue_type(1);
    run_producer_consumer<false>(queue, 10'000);
}

TYPED_TEST_P(QueueTest, MT_ProducerConsumerSmallCapacityLvalueRef) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    if constexpr (std::is_copy_assignable_v<item_type>) {
        auto queue = queue_type(1);
        run_producer_consumer<true>(queue, 10'000);
    }
}

template <bool use_lvalue_push, typename Queue>
void run_cross_queue_worker(Queue& outgoing_queue, Queue& incoming_queue,
                            int num_items) {
    using item_type = typename Queue::value_type;
    auto produced = 0;
    auto consumed = 0;
    auto item = item_type{-1};

    while (produced < num_items || consumed < num_items) {
        auto made_progress = false;
        if (produced < num_items) {
            if constexpr (use_lvalue_push) {
                auto input = item_type{produced};
                if (outgoing_queue.try_push(input)) {
                    ++produced;
                    made_progress = true;
                }
            } else if (outgoing_queue.try_push(item_type{produced})) {
                ++produced;
                made_progress = true;
            }
        }

        if (consumed < num_items && incoming_queue.try_pop(item)) {
            EXPECT_EQ(item, item_type{consumed});
            ++consumed;
            made_progress = true;
        }

        if (!made_progress) {
            std::this_thread::yield();
        }
    }
}

TYPED_TEST_P(QueueTest, MT_CrossQueueProducerConsumer) {
    using queue_type = TypeParam;
    auto queueA = queue_type(8);
    auto queueB = queue_type(3);
    const auto num_items = 100'000;

    auto first_thread = std::thread(
        [&]() { run_cross_queue_worker<false>(queueA, queueB, num_items); });
    auto second_thread = std::thread(
        [&]() { run_cross_queue_worker<false>(queueB, queueA, num_items); });

    first_thread.join();
    second_thread.join();
}

TYPED_TEST_P(QueueTest, MT_CrossQueueProducerConsumerLvalueRef) {
    using queue_type = TypeParam;
    using item_type = typename queue_type::value_type;
    if constexpr (std::is_copy_assignable_v<item_type>) {
        auto queueA = queue_type(8);
        auto queueB = queue_type(3);
        const auto num_items = 100'000;

        auto first_thread = std::thread(
            [&]() { run_cross_queue_worker<true>(queueA, queueB, num_items); });
        auto second_thread = std::thread(
            [&]() { run_cross_queue_worker<true>(queueB, queueA, num_items); });

        first_thread.join();
        second_thread.join();
    }
}

REGISTER_TYPED_TEST_SUITE_P(
    QueueTest, Capacity, ST_TryPopEmpty, ST_TryPushAndPop,
    ST_TryPushAndPopLvalueRef, ST_TryPopEmptyAfterPush, ST_TryPushFull,
    ST_FillThenEmpty, ST_WrapAround, ST_WrapAroundLvalueRef, ST_PushPopCycle,
    ST_PushPopCycleLvalueRef, MT_ProducerConsumer, MT_ProducerConsumerLvalueRef,
    MT_ProducerConsumerStress, MT_ProducerConsumerStressLvalueRef,
    MT_ProducerConsumerSmallCapacity, MT_ProducerConsumerSmallCapacityLvalueRef,
    MT_CrossQueueProducerConsumer, MT_CrossQueueProducerConsumerLvalueRef);

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
