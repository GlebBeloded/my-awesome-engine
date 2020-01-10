
#pragma once
#include "ppm.hpp"

using namespace draw_utils;

namespace triangle {
class renderer : public ppm::image {

    inline std::vector<point> get_line(const point& a, const point& b);
    inline std::vector<point> get_line_high(const point& a, const point& b);
    inline std::vector<point> get_line_low(const point& a, const point& b);

public:
    renderer(int32_t x, int32_t y)
        : ppm::image(x, y) {}

    void draw(const std::vector<point>&  vertex,
              const std::vector<size_t>& indicies, color col);
    void clear(color col = { 0, 0, 0 });
};
} // namespace triangle