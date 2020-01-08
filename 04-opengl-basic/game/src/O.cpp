#include "pieces.hpp"

tetris::O::O()
    : kind{ piece_types::O } {
    data[0].move_to_coords(4, 10);
    data[1].move_to_coords(5, 10);
    data[2].move_to_coords(4, 11);
    data[3].move_to_coords(5, 11);
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
                tile.coords.first -= 1;
            }
            break;
            case eng::event::d_pressed: {
                for (auto& tile : data) {
                    tile.apply_matrix(tetris::move_right);
                    tile.coords.first += 1;
                }
                break;
                case eng::event::s_pressed: {
                    for (auto& tile : data) {
                        tile.apply_matrix(tetris::move_down);
                        tile.coords.second -= 1;
                    }
                    break;
                }
            }
        }
    }
}
void tetris::O::move(int x, int y) {
    data[0].move_to_coords(x, y);
    data[1].move_to_coords(x + 1, y);
    data[2].move_to_coords(x, y + 1);
    data[3].move_to_coords(x + 1, y + 1);
}

std::array<std::pair<int, int>, 4> tetris::O::coords_after_rotation(int) {
    return { {
        { data[0].coords.first, data[0].coords.second },
        { data[1].coords.first, data[1].coords.second },
        { data[2].coords.first, data[2].coords.second },
        { data[3].coords.first, data[3].coords.second },
    } };
}

void tetris::O::set_color(color c) {
    for (auto& tile : data)
        tile.set_color(c);
}
