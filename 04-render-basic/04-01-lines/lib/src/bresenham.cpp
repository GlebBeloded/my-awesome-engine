#include "bresenham.hpp"

void bresenham::draw_low(const draw_utils::point& a, const draw_utils::point& b,
                         const draw_utils::color& color, ppm::image& img) {
    auto dx = b.x - a.x;
    auto dy = b.y - a.y;

    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = a.y;

    for (int x = a.x; x <= b.x; ++x) {
        img.get_pixel({ x, y }) = color;
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void bresenham::draw_high(const draw_utils::point& a,
                          const draw_utils::point& b,
                          const draw_utils::color& color, ppm::image& img) {
    auto dx = b.x - a.x;
    auto dy = b.y - a.y;

    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = 2 * dx - dy;
    int x = a.x;
    for (int y = a.y; y <= b.y; ++y) {
        img.get_pixel({ x, y }) = color;
        if (D > 0) {
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
}

void bresenham::draw_line(const draw_utils::point& a,
                          const draw_utils::point& b,
                          const draw_utils::color& color, ppm::image& img) {
    if (draw_utils::delta(b.y, a.y) < draw_utils::delta(b.x, a.x)) {
        if (a.x > b.x) { // check if second x is larger then first, if so,
                         // swap them, because we need to draw left to right
            draw_low(b, a, color, img);
        } else {
            draw_low(a, b, color, img);
        }
    } else {
        if (a.y > b.y) {
            draw_high(b, a, color, img);
        } else {
            draw_high(a, b, color, img);
        }
    }
}
