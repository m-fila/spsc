#include <gtest/gtest.h>

#include "spsc/detail/wrap_index.hpp"

TEST(DetailWrapIndex, AdvancesAndWraps) {
    EXPECT_EQ(spsc::detail::wrap_index(0, 4), 1);
    EXPECT_EQ(spsc::detail::wrap_index(1, 4), 2);
    EXPECT_EQ(spsc::detail::wrap_index(2, 4), 3);
    EXPECT_EQ(spsc::detail::wrap_index(3, 4), 0);
    EXPECT_EQ(spsc::detail::wrap_index(0, 1), 0);
}
