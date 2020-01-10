#include "renderer.hpp"

using namespace triangle;

std::vector<point> renderer::get_line(const point& a, const point& b) {

    if (a.x > this->get_x_size() || b.x > this->get_x_size() ||
        a.y > this->get_y_size() || b.y > this->get_y_size())
        throw std::runtime_error("pixel coordinates outside of ppm image");

    if (draw_utils::delta(b.y, a.y) < draw_utils::delta(b.x, a.x)) {
        if (a.x > b.x) { // check if second x is larger then first, if so,
                         // swap them, because we need to draw left to right
            return get_line_low(b, a);
        } else {
            return get_line_low(a, b);
        }
    } else {
        if (a.y > b.y) {
            return get_line_high(b, a);
        } else {
            return get_line_high(a, b);
        }
    }
}

std::vector<point> renderer::get_line_low(const draw_utils::point& a,
                                          const draw_utils::point& b) {
    auto               dx = b.x - a.x;
    auto               dy = b.y - a.y;
    std::vector<point> points;

    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = a.y;

    for (int x = a.x; x <= b.x; ++x) {
        points.push_back({ x, y });
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
    return points;
}

std::vector<point> renderer::get_line_high(const draw_utils::point& a,
                                           const draw_utils::point& b) {

    auto               dx = b.x - a.x;
    auto               dy = b.y - a.y;
    std::vector<point> points;

    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = 2 * dx - dy;
    int x = a.x;
    for (int y = a.y; y <= b.y; ++y) {
        points.push_back({ x, y });
        if (D > 0) {
            x = x + xi;
            D = D - 2 * dy;
        }
        D = D + 2 * dx;
    }
    return points;
}

void renderer::draw(const std::vector<point>&  vertex,
                    const std::vector<size_t>& indicies, color col) {

    std::vector<point> pixels;

    for (auto i = 0; i < indicies.size() - 1; i++) {
        auto x = get_line(vertex.at(indicies[i]), vertex.at(indicies[i + 1]));
        pixels.insert(pixels.end(), x.begin(), x.end());
    }
    for (auto position : pixels)
        at(position) = col;
}

void renderer::clear(color col) {
    std::fill(begin(), end(), col);
}