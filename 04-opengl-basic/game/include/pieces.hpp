#pragma once
#include "types.hpp"

namespace tetris {

class O : public piece {
public:
    O();
    virtual piece_types                        type() override { return kind; };
    virtual void                               rotate() override;
    virtual void                               move(eng::event) override;
    virtual void                               move(int x, int y) override;
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(
        int i) override;
    virtual std::array<tile, 4>& get_tiles() override { return data; };
    virtual void                 render(eng::engine*) override;
    virtual void                 set_color(color) override;

private:
    std::array<tile, 4> data;
    piece_types         kind;
};

class I : public piece {
public:
    I();
    virtual piece_types                        type() { return kind; };
    virtual void                               rotate();
    virtual void                               move(eng::event);
    virtual void                               move(int x, int y);
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(int i);
    virtual std::array<tile, 4>&               get_tiles() { return data; };
    virtual void                               render(eng::engine*);
    virtual void                               set_color(color);

private:
    std::array<tile, 4> data;
    piece_types         kind;
    uint                position_iterator;
    constexpr static std::array<std::array<std::pair<int, int>, 4>, 4>
        rotations{ {
            { { { 2, -1 }, { 1, 0 }, { 0, 1 }, { -1, 2 } } },
            { { { 1, 2 }, { 0, 1 }, { -1, 0 }, { -2, -1 } } },
            { { { -2, 0 }, { -1, -1 }, { 0, -2 }, { 1, 1 } } },
            { { { -1, -1 }, { 0, 0 }, { 1, 1 }, { 2, -2 } } },
        } };
};

class T : public piece {
public:
    T();
    virtual piece_types                        type() { return kind; };
    virtual void                               rotate();
    virtual void                               move(eng::event);
    virtual void                               move(int x, int y);
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(int i);
    virtual std::array<tile, 4>&               get_tiles() { return data; };
    virtual void                               render(eng::engine*);
    virtual void                               set_color(color);

private:
    std::array<tile, 4> data;
    piece_types         kind;
    uint                position_iterator;
    constexpr static std::array<std::array<std::pair<int, int>, 4>, 4>
        rotations{ {
            { { { 1, -1 }, { 0, 0 }, { -1, 1 }, { -1, -1 } } },
            { { { 1, 1 }, { 0, 0 }, { -1, -1 }, { 1, -1 } } },
            { { { -1, 1 }, { 0, 0 }, { 1, -1 }, { 1, 1 } } },
            { { { -1, -1 }, { 0, 0 }, { 1, 1 }, { -1, 1 } } },
        } };
};

class L : public piece {
public:
    L();
    virtual piece_types                        type() { return kind; };
    virtual void                               rotate();
    virtual void                               move(eng::event);
    virtual void                               move(int x, int y);
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(int i);
    virtual std::array<tile, 4>&               get_tiles() { return data; };
    virtual void                               render(eng::engine*);
    virtual void                               set_color(color);

private:
    std::array<tile, 4> data;
    piece_types         kind;
    uint                position_iterator;
    constexpr static std::array<std::array<std::pair<int, int>, 4>, 4>
        rotations{ {
            { { { 1, -1 }, { 0, 0 }, { -1, 1 }, { 0, -2 } } },
            { { { 1, 1 }, { 0, 0 }, { -1, -1 }, { 2, 0 } } },
            { { { -1, 1 }, { 0, 0 }, { 1, -1 }, { 0, 2 } } },
            { { { -1, -1 }, { 0, 0 }, { 1, 1 }, { -2, 0 } } },
        } };
};

class J : public piece {
public:
    J();
    virtual piece_types                        type() { return kind; };
    virtual void                               rotate();
    virtual void                               move(eng::event);
    virtual void                               move(int x, int y);
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(int i);
    virtual std::array<tile, 4>&               get_tiles() { return data; };
    virtual void                               render(eng::engine*);
    virtual void                               set_color(color);

private:
    std::array<tile, 4> data;
    piece_types         kind;
    uint                position_iterator;
    constexpr static std::array<std::array<std::pair<int, int>, 4>, 4>
        rotations{ {
            { { { 1, -1 }, { 0, 0 }, { -1, 1 }, { -2, 0 } } },
            { { { 1, 1 }, { 0, 0 }, { -1, -1 }, { 0, -2 } } },
            { { { -1, 1 }, { 0, 0 }, { 1, -1 }, { 2, 0 } } },
            { { { -1, -1 }, { 0, 0 }, { 1, 1 }, { 0, 2 } } },
        } };
};

class S : public piece {
public:
    S();
    virtual piece_types                        type() { return kind; };
    virtual void                               rotate();
    virtual void                               move(eng::event);
    virtual void                               move(int x, int y);
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(int i);
    virtual std::array<tile, 4>&               get_tiles() { return data; };
    virtual void                               render(eng::engine*);
    virtual void                               set_color(color);

private:
    std::array<tile, 4> data;
    piece_types         kind;
    uint                position_iterator;
    constexpr static std::array<std::array<std::pair<int, int>, 4>, 4>
        rotations{ {
            { { { 1, -1 }, { 0, 0 }, { -1, -1 }, { -2, 0 } } },
            { { { 1, 1 }, { 0, 0 }, { 1, -1 }, { 0, -2 } } },
            { { { -1, 1 }, { 0, 0 }, { 1, 1 }, { 2, 0 } } },
            { { { -1, -1 }, { 0, 0 }, { -1, 1 }, { 0, 2 } } },
        } };
};

class Z : public piece {
public:
    Z();
    virtual piece_types                        type() { return kind; };
    virtual void                               rotate();
    virtual void                               move(eng::event);
    virtual void                               move(int x, int y);
    virtual std::array<std::pair<int, int>, 4> coords_after_rotation(int i);
    virtual std::array<tile, 4>&               get_tiles() { return data; };
    virtual void                               render(eng::engine*);
    virtual void                               set_color(color);

private:
    std::array<tile, 4> data;
    piece_types         kind;
    uint                position_iterator;
    constexpr static std::array<std::array<std::pair<int, int>, 4>, 4>
        rotations{ {
            { { { 1, -1 }, { 0, 0 }, { 1, 1 }, { 0, 2 } } },
            { { { 1, 1 }, { 0, 0 }, { -1, 1 }, { -2, 0 } } },
            { { { -1, 1 }, { 0, 0 }, { -1, -1 }, { 0, -2 } } },
            { { { -1, -1 }, { 0, 0 }, { 1, -1 }, { 2, 0 } } },
        } };
};
} // namespace tetris