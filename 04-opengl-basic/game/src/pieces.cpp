#include "pieces.hpp"

tetris::O::O()
    : kind{ piece_type::O } {
    data[0].set_coords(3, 4);
    data[1].set_coords(4, 4);
    data[2].set_coords(3, 5);
    data[3].set_coords(4, 5);
}

void tetris::O::rotate() {
    for (auto& tile : data) {
        tile.rotate();
    }
}

void tetris::O::render(eng::engine* e) {
    for (auto& tile : data) {
        tile.render(e);
    }
}

void tetris::O::move(eng::event e) {
    switch (e) {
        case eng::event::a_pressed: {
            for (auto& tile : data) {
                tile.apply_matrix(tetris::move_left);
                tile.coords.first -= 1.f;
            }
            break;
            case eng::event::d_pressed: {
                for (auto& tile : data) {
                    tile.apply_matrix(tetris::move_right);
                    tile.coords.first += 1.f;
                }
                break;
                case eng::event::s_pressed: {
                    for (auto& tile : data) {
                        tile.apply_matrix(tetris::move_down);
                        tile.coords.second -= 1.f;
                    }
                    break;
                }
            }
        }
    }
}