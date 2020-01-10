#include "types.hpp"
#include <tuple>

using namespace draw_utils;

std::tuple<std::vector<point>, std::vector<size_t>> triangle_net(int x, int y,
                                                                 int x_max,
                                                                 int y_max) {
    auto       tri_in_a_row    = x_max / x;
    const auto tri_in_a_column = y_max / y;

    std::vector<point> verticies;

    std::vector<size_t> indicies(x * y);

    for (auto i = 0; i < tri_in_a_column; i++) {
        for (auto k = 0; k < tri_in_a_row; k++) {
            verticies.push_back(
                { k * x, y * i }); // this matrix contains a pixel field, i.e.
                                   // dots that need to be connected
        }
        verticies.push_back({ x_max - 1, y * i });
    }

    for (auto i = 0; i < tri_in_a_row; i++)
        verticies.push_back(
            { i * x,
              y_max - 1 }); // add the last line, excpet for the last pixel

    verticies.push_back(
        { x_max - 1, y_max - 1 }); // furthest pixel on the screen

    for (auto i = 0; i < tri_in_a_column; i++) {
        size_t C; // C is outside of for scope because we need to return to the
                  // 0x pixel becouse of how our draw function works
        // random plus ones refer to the x_max y_max lines we added
        // so the image would not have empty triangles
        for (auto k = 0; k < tri_in_a_row; k++) {
            size_t A =
                (tri_in_a_row + 1) * i + k; // refer to notes in sheet papers
            size_t B = A + tri_in_a_row + 1;
            C        = B + 1;
            size_t D = A + 1;
            indicies.insert(indicies.end(), { A, B, C, A, D });
        }
        indicies.push_back(C);
    }
    return { verticies, indicies };
}