#pragma once
#include "types.hpp"

namespace tetris {

class O : public piece {
public:
    O();
    virtual piece_type          type() { return kind; };
    virtual void                rotate();
    virtual void                move(eng::event);
    virtual std::array<tile, 4> get_tiles() { return data; };
    virtual void                render(eng::engine*);

private:
    std::array<tile, 4> data;
    piece_type          kind;
};

} // namespace tetris