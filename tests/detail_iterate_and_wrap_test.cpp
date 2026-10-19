#include <gtest/gtest.h>

#include "spsc/detail/iterate_and_wrap.hpp"

TEST(DetailIterateAndWrap, AdvancesAndWraps) {
    EXPECT_EQ(spsc::detail::iterate_and_wrap(0, 4), 1);
    EXPECT_EQ(spsc::detail::iterate_and_wrap(1, 4), 2);
    EXPECT_EQ(spsc::detail::iterate_and_wrap(2, 4), 3);
    EXPECT_EQ(spsc::detail::iterate_and_wrap(3, 4), 0);
    EXPECT_EQ(spsc::detail::iterate_and_wrap(0, 1), 0);
}
