#include "renderer.hpp"

using namespace triangle;

void renderer::draw_line(point a, point b, color col) {

    if (a.x > this->get_x_size() || b.x > this->get_x_size() ||
        a.y > this->get_y_size() || b.y > this->get_y_size())
        throw std::runtime_error("pixel coordinates outside of ppm image");

    if (draw_utils::delta(b.y, a.y) < draw_utils::delta(b.x, a.x)) {
        if (a.x > b.x) { // check if second x is larger then first, if so,
                         // swap them, because we need to draw left to right
            draw_low(b, a, col);
        } else {
            draw_low(a, b, col);
        }
    } else {
        if (a.y > b.y) {
            draw_high(b, a, col);
        } else {
            draw_high(a, b, col);
        }
    }
}

void renderer::draw_low(const draw_utils::point& a, const draw_utils::point& b,
                        const draw_utils::color& col) {
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
        this->at({ x, y }) = col;
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void renderer::draw_high(const draw_utils::point& a, const draw_utils::point& b,
                         const draw_utils::color& col) {

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
        this->at({ x, y }) = col;
        if (D > 0) {
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
}

void renderer::draw_triangle_lines(point a, point b, point c, color col) {
    this->draw_line(a, b, col);
    this->draw_line(b, c, col);
    this->draw_line(c, a, col);
}

void renderer::clear(color col) {
    std::fill(begin(), end(), col);
}