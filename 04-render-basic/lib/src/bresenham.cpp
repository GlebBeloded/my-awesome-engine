#include "bresenham.hpp"

void bresenham::draw_low(int x0, int y0, int x1, int y1,
                         const draw_utils::color& color, ppm::image& img) {
    auto dx = x1 - x0;
    auto dy = y1 - y0;

    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = y0;

    for (int x = x0; x <= x1; ++x) {
        img.at(img.get_x_size() * y + x) = color;
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void bresenham::draw_high(int x0, int y0, int x1, int y1,
                          const draw_utils::color& color, ppm::image& img) {
    auto dx = x1 - x0;
    auto dy = y1 - y0;

    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = 2 * dx - dy;
    int x = x0;
    for (int y = y0; y <= y1; ++y) {
        img.at(img.get_x_size() * y + x) = color;
        if (D > 0) {
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
}

void bresenham::draw_line(int x0, int y0, int x1, int y1,
                          const draw_utils::color& color, ppm::image& img) {
    if (draw_utils::delta(y1, y0) < draw_utils::delta(x1, x0)) {
        if (x0 > x1) { // check if second x is larger then first, if so,
                       // swap them, because we need to draw left to right
            draw_low(x1, y1, x0, y0, color, img);
        } else {
            draw_low(x0, y0, x1, y1, color, img);
        }
    } else {
        if (y0 > y1) {
            draw_high(x1, y1, x0, y0, color, img);
        } else {
            draw_high(x0, y0, x1, y1, color, img);
        }
    }
}
