#pragma once

#include "types.hpp"
#include <fstream>
#include <string>
#include <vector>

// std::filesystem::create_directory

namespace ppm {
using namespace draw_utils;

class image : public std::vector<color> {
    int32_t x_size;
    int32_t y_size;

public:
    int32_t get_x_size() { return x_size; };
    int32_t get_y_size() { return y_size; };

    image(int32_t x, int32_t y)
        : x_size{ x }
        , y_size{ y } {
        this->resize(x * y);
    }

    std::ofstream write_image(const std::string& path);
    std::ifstream load_image(const std::string& path);
}; // namespace ppm

} // namespace ppm
