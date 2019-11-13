#include "types.hpp"
#include <tuple>

using namespace draw_utils;

std::tuple<std::vector<point>, std::vector<size_t>> triangle_net(int x, int y,
                                                                 int x_max,
                                                                 int y_max) {
    auto tri_in_a_row    = (x_max - 1) / x;
    auto tri_in_a_column = (y_max - 1) / y;

    std::vector<point> vertexes;

    std::vector<size_t> indexes;

    for (auto i = 0; i < tri_in_a_column; i++) {
        for (auto k = 0; k < tri_in_a_row; k++) {
            vertexes.push_back({ k * tri_in_a_row, y * tri_in_a_column });
        }
    }
    for (auto i = 0; i < tri_in_a_column; i++) {
        for (auto k = 0; k < tri_in_a_row; k++) {
            auto index_0 = k * tri_in_a_row;
            indexes.push_back(0);
            indexes.push_back(0);
        }
    }
    return { vertexes, indexes };
}