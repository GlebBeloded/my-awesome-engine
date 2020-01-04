#include "types.hpp"
#include <type_traits>

using namespace tetris;

template <typename E>
constexpr auto to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

tile::tile(std::pair<float,float>_coords, tetris::color col, const matrix::matrix& m)
    : coords{_coords}
    {

    init_tex_coord();
    set_color(col);
    apply_matrix(m);
}

inline void tile::apply_matrix(const matrix::matrix& m) {
    for (auto& triangle : data) {
        for (auto& vertex : triangle.v) {
            vertex.coord = vertex.coord * m;
        }
    }
}

inline void tile::set_color(tetris::color col) {
    color = tetris::colors.at(to_underlying(col));
    for (auto& vertex_array : data) {
        for (auto& vertex : vertex_array.v) {
            vertex.color = color;
        }
    }
}
inline void tile::init_tex_coord() {
    data[0].v[0].tx = 0.f;
    data[0].v[0].ty = 1.f;
    data[0].v[1].tx = 1.f;
    data[0].v[1].ty = 1.f;
    data[0].v[2].tx = 1.f;
    data[0].v[2].ty = 0.f;

    data[1].v[0].tx = 0.f;
    data[1].v[0].ty = 1.f;
    data[1].v[1].tx = 1.f;
    data[1].v[1].ty = 0.f;
    data[1].v[2].tx = 0.f;
    data[1].v[2].ty = 0.f;
}

void tile::rotate() {
    apply_matrix(matrix::move(tetris::zero_vector));
    apply_matrix(matrix::counter_clockwise_90());
    apply_matrix(matrix::move(matrix::vector{coords.first,coords.second}));
}

void tile::render(eng::engine* k){
    k->render_triangle(data.at(0));
    k->render_triangle(data.at(1));
}