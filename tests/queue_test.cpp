#include "spsc/queue.hpp"

#include <memory>

#include "custom_types.hpp"
#include "queue_test_common.hpp"

template <typename T, spsc::capacity_policy Policy>
using queue_type = spsc::queue<T, std::allocator<T>, Policy>;

#define SPSC_INSTANTIATE_QUEUE_W_POLICY_TESTS(prefix, policy)                 \
    using prefix##Types = ::testing::Types<                                   \
        queue_type<int, spsc::capacity_policy::policy>,                       \
        queue_type<custom_move_only_type, spsc::capacity_policy::policy>>;    \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix, QueueTest, prefix##Types);         \
    using prefix##AllocatorTypes = ::testing::Types<                          \
        spsc::queue<int, std::pmr::polymorphic_allocator<int>,                \
                    spsc::capacity_policy::policy>>;                          \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix##Allocator, QueueAllocatorTest,     \
                                   prefix##AllocatorTypes);                   \
    using prefix##DestructionTypes = ::testing::Types<                        \
        queue_type<custom_tracked_type, spsc::capacity_policy::policy>>;      \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix##Destruction, QueueDestructionTest, \
                                   prefix##DestructionTypes);                 \
    using prefix##EmplaceTypes =                                              \
        ::testing::Types<queue_type<custom_move_constructible_only_type,      \
                                    spsc::capacity_policy::policy>>;          \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix##Emplace, QueueEmplaceTest,         \
                                   prefix##EmplaceTypes);                     \
    using prefix##ConstructionThrowTypes =                                    \
        ::testing::Types<queue_type<custom_throwing_construct_type,           \
                                    spsc::capacity_policy::policy>>;          \
    INSTANTIATE_TYPED_TEST_SUITE_P(prefix##ConstructionThrowing,              \
                                   QueueConstructionThrowTest,                \
                                   prefix##ConstructionThrowTypes)

SPSC_INSTANTIATE_QUEUE_W_POLICY_TESTS(queueCapacityExact, exact);
SPSC_INSTANTIATE_QUEUE_W_POLICY_TESTS(queueCapacityMinimum, minimum);

#undef SPSC_INSTANTIATE_QUEUE_W_POLICY_TESTS
