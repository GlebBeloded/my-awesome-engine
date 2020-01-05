#pragma once
#include "engine.hpp"
#include "matrix.hpp"
#include "state.hpp"
#include <array>
#include <string_view>

namespace tetris {
enum class color { red = 0, blue, green, purple, gray };
enum class piece_type { I, J, L, O, S, T, Z };

constexpr static std::array<eng::rgb, 5> colors{
    { { 240.f / 255.f, 128.f / 255.f, 128.f / 255.f },
      { 100.f / 255.f, 149.f / 255.f, 237.f / 255.f },
      { 60.f / 255.f, 179.f / 255.f, 113.f / 255.f },
      { 186.f / 255.f, 85.f / 255.f, 211.f / 255.f },
      { 216.f / 255.f, 191.f / 255.f, 216.f / 255.f } }
};

// make all of these calculatable at compile time
constexpr matrix::vector zero_vector{};
constexpr matrix::matrix move_left{ matrix::move(matrix::vector{ -.2f, 0.f }) };
constexpr matrix::matrix move_right{ matrix::move(matrix::vector{ .2f, 0.f }) };
constexpr matrix::matrix move_down{ matrix::move(matrix::vector{ 0.f, -.1f }) };
// displacement_from_center represents how much do we need to move one piece by
// x and by y so it will fit into a square
constexpr std::pair<float, float> displacement_from_center{
    (state::coordinate_system_center.x - (2.f / state::board_size.x / 2.f)),
    (state::coordinate_system_center.y - (2.f / state::board_size.y / 2.f))
};

class tile {
public:
    tile();
    void apply_matrix(const matrix::matrix& m);
    void set_color(color col);
    // only rotates texture
    void                    rotate();
    void                    render(eng::engine*);
    void                    set_coords(float x, float y);
    std::pair<float, float> coords; // change to int
    std::pair<float, float> position();

private:
    void                         init_tex_coord();
    void                         normalize();
    eng::rgb                     color;
    std::array<eng::triangle, 2> data;
};

class piece {

public:
    piece() {}
    virtual piece_type          type()               = 0;
    virtual void                rotate()             = 0;
    virtual void                move(eng::event)     = 0;
    virtual std::array<tile, 4> get_tiles()          = 0;
    virtual void                render(eng::engine*) = 0;
};

} // namespace tetris