#pragma once

#include "renderer.hpp"

// we could just skip to the next line if triangles don't fit perfectly, but why
// if we can just throw and terminate
void draw_triangle_net(triangle::renderer& img, int width, int height,
                       const color& col) {
    if (img.get_x_size() % width != 0 || img.get_y_size() % height != 0)
        throw std::logic_error("invalid triangle net params");

    const auto x_step = img.get_x_size() / width;
    const auto y_step = img.get_y_size() / height;

    for (auto column = 0; img.get_y_size() - y_step != y_step * column;
         column++) {
        for (auto row = 0; img.get_x_size() - x_step > row * x_step; row++) {
            img.draw_triangle_lines( // this draws normal 90 degrees triangle
                { x_step * row, y_step * column },
                { x_step * row + x_step, y_step * column + y_step },
                { x_step * row, y_step * column + y_step }, col);

            img.draw_triangle_lines( // this is A1,B1, {B1.x,A1.Y} triangle
                { x_step * row, y_step * column },
                { x_step * row + x_step, y_step * column + y_step },
                { x_step * row + x_step, y_step * column }, col);
        }
    }
}