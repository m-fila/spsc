#include "spsc/detail/next_power2.hpp"

#include <gtest/gtest.h>

TEST(Details, NextPower2) {
    EXPECT_EQ(spsc::detail::next_power2(0u), 2u);
    EXPECT_EQ(spsc::detail::next_power2(1u), 2u);
    EXPECT_EQ(spsc::detail::next_power2(2u), 4u);
    EXPECT_EQ(spsc::detail::next_power2(3u), 4u);
    EXPECT_EQ(spsc::detail::next_power2(4u), 4u);
    EXPECT_EQ(spsc::detail::next_power2(5u), 8u);
    EXPECT_EQ(spsc::detail::next_power2(6u), 8u);
    EXPECT_EQ(spsc::detail::next_power2(7u), 8u);
    EXPECT_EQ(spsc::detail::next_power2(8u), 16u);
    EXPECT_EQ(spsc::detail::next_power2(9u), 16u);
    EXPECT_EQ(spsc::detail::next_power2(123u), 128u);
    EXPECT_EQ(spsc::detail::next_power2(1027u), 2048u);
}
