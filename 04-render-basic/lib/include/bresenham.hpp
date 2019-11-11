#pragma once

#include "ppm.hpp"

namespace bresenham {
void draw_line(int x0, int y0, int x1, int y1, const draw_utils::color& color,
               ppm::image& img);
void draw_high(int x0, int y0, int x1, int y1, const draw_utils::color& color,
               ppm::image& img);
void draw_low(int x0, int y0, int x1, int y1, const draw_utils::color& color,
              ppm::image& img);
} // namespace bresenham