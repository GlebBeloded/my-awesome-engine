#pragma once

namespace state {
struct BoardSize {
    float x, y;
};
constexpr static BoardSize board_size{ 10, 20 };
constexpr static BoardSize coordinate_system_center{ 0, 0 };
} // namespace state