#include "pieces.hpp"

tetris::L::L()
    : kind{ piece_types::L } {
    data[0].move_to_coords(4, 20); // A
    data[1].move_to_coords(5, 20); // B CENTERPIECE
    data[2].move_to_coords(6, 20); // C
    data[3].move_to_coords(4, 21); // D
    position_iterator = 0;
}

void tetris::L::rotate() {

    auto displacements = rotations[position_iterator % 4];
    position_iterator++;

    data[0].move_to_coords(data[0].coords.first + displacements[0].first,
                           data[0].coords.second + displacements[0].second);
    data[1].move_to_coords(data[1].coords.first + displacements[1].first,
                           data[1].coords.second + displacements[1].second);
    data[2].move_to_coords(data[2].coords.first + displacements[2].first,
                           data[2].coords.second + displacements[2].second);
    data[3].move_to_coords(data[3].coords.first + displacements[3].first,
                           data[3].coords.second + displacements[3].second);

    for (auto& tile : data) {
        tile.rotate();
    }
}

void tetris::L::render(eng::engine* e) {
    for (auto& tile : data) {
        tile.render(e);
    }
}

void tetris::L::move(eng::event e) {
    switch (e) {
        case eng::event::a_pressed:
            for (auto& tile : data) {
                tile.apply_matrix(tetris::move_left);
                tile.coords.first -= 1;
            }
            break;
        case eng::event::d_pressed:
            for (auto& tile : data) {
                tile.apply_matrix(tetris::move_right);
                tile.coords.first += 1;
            }
            break;
        case eng::event::s_pressed:
            for (auto& tile : data) {
                tile.apply_matrix(tetris::move_down);
                tile.coords.second -= 1;
            }
            break;
        default:
            break;
    }
}
void tetris::L::move(int x, int y) {
    // data[1] is figure centerpiece
    auto displacement = calculate_displacement(data[1].coords, { x, y });

    data[0].move_to_coords(data[0].coords.first + displacement.first,
                           data[0].coords.second + displacement.second);
    data[1].move_to_coords(data[1].coords.first + displacement.first,
                           data[1].coords.second + displacement.second);
    data[2].move_to_coords(data[2].coords.first + displacement.first,
                           data[2].coords.second + displacement.second);
    data[3].move_to_coords(data[3].coords.first + displacement.first,
                           data[3].coords.second + displacement.second);
}

std::array<std::pair<int, int>, 4> tetris::L::coords_after_rotation() {
    auto& displacements = rotations[position_iterator % 4];

    return { {
        { data[0].coords.first + displacements[0].first,
          data[0].coords.second + displacements[0].second },
        { data[1].coords.first + displacements[1].first,
          data[1].coords.second + displacements[1].second },
        { data[2].coords.first + displacements[2].first,
          data[2].coords.second + displacements[2].second },
        { data[3].coords.first + displacements[3].first,
          data[3].coords.second + displacements[3].second },
    } };
}

void tetris::L::set_color(color c) {
    for (auto& tile : data)
        tile.set_color(c);
}
