#include "types.hpp"
#include "state.hpp"
#include <stdio.h>
#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

inline std::pair<float, float> tetris::tile::position() {
    return { (data[1].v[2].coord.x + data[0].v[1].coord.x) / 2.f,
             (data[1].v[2].coord.y + data[0].v[1].coord.y) / 2.f };
}

inline void tetris::tile::apply_matrix(const matrix::matrix& m) {
    for (auto& triangle : data) {
        for (auto& vertex : triangle.v) {
            vertex.coord = vertex.coord * m;
        }
    }
}

inline void tetris::tile::set_color(tetris::color col) {
    color = tetris::colors.at(to_underlying(col));
    for (auto& vertex_array : data) {
        for (auto& vertex : vertex_array.v) {
            vertex.color = color;
        }
    }
}

inline void tetris::tile::init_tex_coord() {
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

inline void tetris::tile::normalize() {
    data[0].v[0].coord.x = -0.5;
    data[0].v[0].coord.y = 0.5;

    data[0].v[1].coord.x = 0.5;
    data[0].v[1].coord.y = 0.5;

    data[0].v[2].coord.x = 0.5;
    data[0].v[2].coord.y = -0.5;

    data[1].v[0].coord.x = -0.5;
    data[1].v[0].coord.y = 0.5;

    data[1].v[1].coord.x = 0.5;
    data[1].v[1].coord.y = -0.5;

    data[1].v[2].coord.x = -0.5;
    data[1].v[2].coord.y = -0.5;

    apply_matrix(
        matrix::move({ displacement_from_center.first,
                       displacement_from_center.second }) *
        matrix::scale(2.f / state::board_size.x, 2.f / state::board_size.y));
    coords = { 5, 10 };
}

tetris::tile::tile()
    : coords{ 0.f, 0.f } {
    init_tex_coord();
    normalize();
    set_color(tetris::color::gray);
}

void tetris::tile::rotate() {
    auto displacement = calculate_displacement(tetris::zero_vector, position());
    apply_matrix(matrix::move({ -displacement.first, -displacement.second }));
    apply_matrix(matrix::counter_clockwise_90());
    // each time we rotate a piece we have to scale it because of weird tetris
    // proporsions
    apply_matrix(matrix::scale(2.f, 0.5f));
    apply_matrix(matrix::move({ displacement.first, displacement.second }));
}

void tetris::tile::render(eng::engine* k) {
    k->render_triangle(data.at(0));
    k->render_triangle(data.at(1));
}

void tetris::tile::move_to_coords(int x, int y) {

    if (x < 1 || x > state::board_size.x)
        throw std::logic_error("x coordinates outside of field");

    auto displacement = calculate_displacement(coords, { x, y });

    apply_matrix(
        matrix::move({ displacement.first * singular_displacement.first,
                       displacement.second * singular_displacement.second }));
    coords = { x, y };
};