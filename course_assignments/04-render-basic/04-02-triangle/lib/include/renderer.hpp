
#pragma once
#include "ppm.hpp"

using namespace draw_utils;

namespace triangle {
class renderer : public ppm::image {

    void draw_line(point a, point b, color col);
    void draw_high(const point& a, const point& b, const color& col);
    void draw_low(const point& a, const point& b, const color& color);

public:
    renderer(int32_t x, int32_t y)
        : ppm::image(x, y) {}

    void draw_triangle_lines(point a, point b, point c, color col);
    void clear(color col = { 0, 0, 0 });
    // void draw_triangle(point a, point b, point c, color col);
};
} // namespace triangle