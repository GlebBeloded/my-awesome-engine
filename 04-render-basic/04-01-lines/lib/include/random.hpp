#pragma once

#include "types.hpp"
#include <filesystem>
#include <random>

draw_utils::color random_color() {
    std::random_device                     rd;
    std::mt19937                           mt(rd());
    std::uniform_int_distribution<uint8_t> dist{ 0, 255 };
    return { dist(mt), dist(mt), dist(mt) };
}

draw_utils::point random_point(int32_t x_max, int32_t y_max) {
    std::random_device                     rd;
    std::mt19937                           mt(rd());
    std::uniform_int_distribution<int32_t> x_dist{ 0, x_max };
    std::uniform_int_distribution<int32_t> y_dist{ 0, y_max };

    return { x_dist(mt), y_dist(mt) };
}
