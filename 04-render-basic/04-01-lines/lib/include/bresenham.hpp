#pragma once

#include "ppm.hpp"

namespace bresenham {
void draw_line(const draw_utils::point& a, const draw_utils::point& b,
               const draw_utils::color& color, ppm::image& img);
void draw_high(const draw_utils::point& a, const draw_utils::point& b,
               const draw_utils::color& color, ppm::image& img);
void draw_low(const draw_utils::point& a, const draw_utils::point& b,
              const draw_utils::color& color, ppm::image& img);
} // namespace bresenham