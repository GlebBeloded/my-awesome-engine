#include "engine.hpp"
#include "matrix.hpp"
#include "string_view"
#include <array>

namespace tetris {
enum class color { red = 0, blue, green, purple, gray };

constexpr static std::array<eng::rgb, 5> colors{
    { { 240.f / 255.f, 128.f / 255.f, 128.f / 255.f },
      { 100.f / 255.f, 149.f / 255.f, 237.f / 255.f },
      { 60.f / 255.f, 179.f / 255.f, 113.f / 255.f },
      { 186.f / 255.f, 85.f / 255.f, 211.f / 255.f },
      { 216.f / 255.f, 191.f / 255.f, 216.f / 255.f } }
};

class tile {
public:
    tile(int _x, int _y, color col, const matrix::matrix& m);
    void apply_matrix(const matrix::matrix& m);
    void set_color(color col);

private:
    void                         init_tex_coord();
    int                          x, y;
    eng::rgb                     color;
    std::array<eng::triangle, 2> data;
};

class piece {

public:
    piece(std::array<tile, 4> _data)
        : data{ _data } {}
    virtual std::string_view    type()           = 0;
    virtual void                rotate()         = 0;
    virtual void                move(eng::event) = 0;
    virtual std::array<tile, 4> get_tiles()      = 0;

private:
    std::array<tile, 4> data;
};

} // namespace tetris