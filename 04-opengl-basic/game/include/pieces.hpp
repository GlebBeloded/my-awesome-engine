#include "types.hpp"

namespace tetris{

class O : public piece{
    O(std::array<tile, 4> _data)
        : piece{_data} {}
    virtual std::string_view    type()           = 0;
    virtual void                rotate()         = 0;
    virtual void                move(eng::event) = 0;
    virtual std::array<tile, 4> get_tiles()      = 0;

private:
    std::array<tile, 4> data;
};

}