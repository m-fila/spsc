#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include "spsc/detail/bit.hpp"

TEST(DetailsBit, MaxPower2) {
    static_assert(spsc::detail::max_power2<std::uint8_t>() == 0x80ull);
    static_assert(spsc::detail::max_power2<std::uint16_t>() == 0x8000ull);
    static_assert(spsc::detail::max_power2<std::uint32_t>() == 0x80000000ull);
    static_assert(spsc::detail::max_power2<std::uint64_t>() ==
                  0x8000000000000000ull);
}

TEST(DetailsBit, NextPower2) {
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(0)), std::size_t(2));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(1)), std::size_t(2));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(2)), std::size_t(4));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(3)), std::size_t(4));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(4)), std::size_t(8));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(5)), std::size_t(8));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(6)), std::size_t(8));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(7)), std::size_t(8));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(8)), std::size_t(16));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(9)), std::size_t(16));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(123)), std::size_t(128));
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(1027)), std::size_t(2048));
    EXPECT_EQ(spsc::detail::next_power2((std::size_t(1) << 14) - 1),
              std::size_t(1) << 14);
    EXPECT_EQ(spsc::detail::next_power2(std::size_t(1) << 14), std::size_t(1)
                                                                   << 15);
    EXPECT_THROW(
        spsc::detail::next_power2(std::numeric_limits<std::size_t>::max()),
        std::overflow_error);
}
