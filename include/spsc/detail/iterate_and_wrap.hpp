#pragma once

#include <cstddef>

namespace spsc::detail {

inline std::size_t iterate_and_wrap(std::size_t index,
                                    std::size_t buffer_size) noexcept {
    ++index;
    return index == buffer_size ? 0 : index;
}

}  // namespace spsc::detail
