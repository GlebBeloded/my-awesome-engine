#include "tetris.hpp"

namespace local_shapes {
typedef std::array<std::pair<float, float>, 4> rectangle;
typedef std::array<std::pair<float, float>, 3> triangle;
} // namespace local_shapes
local_shapes::rectangle piece_rectangle_coords(tetris::piece& p) {
    float x_min, x_max, y_min, y_max;
    x_min = y_min = 1;
    x_max = y_max = -1;
    for (const auto& tile : p.get_tiles()) {
        for (const auto& triangle : tile.get_data()) {
            for (const auto& vertex : triangle.v) {
                if (vertex.coord.x > x_max)
                    x_max = vertex.coord.x;
                if (vertex.coord.x < x_min)
                    x_min = vertex.coord.x;
                if (vertex.coord.y > y_max)
                    y_max = vertex.coord.y;
                if (vertex.coord.y < y_min)
                    y_min = vertex.coord.y;
            }
        }
    }
    return { { { x_min, y_min },
               { x_min, y_max },
               { x_max, y_max },
               { x_max, y_min } } };
}

std::pair<float, float> piece_center(tetris::piece& p) {
    auto rect = piece_rectangle_coords(p);
    return { (rect[0].first + rect[2].first) / 2,
             (rect[0].second + rect[2].second) / 2 };
}

float triangle_area(const local_shapes::triangle& r) {
    return std::abs((r[0].first * (r[1].second - r[2].second) +
                     r[1].first * (r[2].second - r[0].second) +
                     r[2].first * (r[0].second - r[1].second)) /
                    2.0);
}

bool is_inside_rectangle(const local_shapes::rectangle& r,
                         const std::pair<float, float>& p) {
    float A = triangle_area({ r[0], r[1], r[2] }) +
              triangle_area({ r[0], r[3], r[2] });

    float A1 = triangle_area({ p, r[0], r[1] });

    float A2 = triangle_area({ p, r[1], r[2] });

    float A3 = triangle_area({ p, r[2], r[3] });

    float A4 = triangle_area({ p, r[0], r[3] });

    return (A == A1 + A2 + A3 + A4);
}

inline std::pair<float, float> opegl_normalize(
    const std::pair<float, float>& sdl_touch_coords) {
    return { sdl_touch_coords.first * (1 - -1) + -1,
             sdl_touch_coords.second * (1 - -1) + -1 };
}

inline std::pair<float, float> zero_one_normalize(
    const std::pair<float, float>& opengl_coords) {
    return { (opengl_coords.first - -1) / (1 - -1),
             (opengl_coords.second - -1) / (1 - -1) };
}
inline std::pair<float, float> finger_displacement(
    const std::pair<float, float>& A, const std::pair<float, float>& B) {
    return { A.first - B.first, A.second - B.second };
}

// f for finger
inline tetris::directions direction_from_displacement(
    std::pair<float, float> piece_center,
    std::pair<float, float> finger_coords) {
    // this is needed to see if X or Y displacement is larger
    auto test = zero_one_normalize(finger_coords);

    finger_coords.second = 1 - finger_coords.second;

    auto displacement =
        finger_displacement(finger_coords, zero_one_normalize(piece_center));

    // if X displacement is bigger than Y displacement
    if (std::abs(displacement.first) > std::abs(displacement.second)) {
        if (displacement.first > 0)
            return tetris::directions::right;
        if (displacement.first < 0)
            return tetris::directions::left;
    } else {

        if (displacement.second > 0.15)
            return tetris::directions::rotate;
        if (displacement.second < -0.2)
            return tetris::directions::down;
    }
    return tetris::directions::_plug;
}

void tetris::game::handle_input() {
    eng::input_data input{};

    while (engine->read_input(input)) {
        // no input can be taken if no piece is there to move
        if (!current_piece)
            return;

        if (input.eventType == eng::event_type::key &&
            input.key_event.has_value()) {
            handle_key(input);
            return;
        }
        if (input.eventType == eng::event_type::touch &&
            input.touch_event.has_value()) {
            handle_touch(input);
            return;
        }
    }
}

void tetris::game::handle_touch(const eng::input_data& input) {

    // logic simplified so I can finish it until tommorow
    // ideally there should also be logic for handling fingermove event
    // if (input.touch_event.value() == eng::touch::finger_down) {
    // if (finger_down_pos.has_value())
    // return;
    // finger_down_pos.emplace(input.f_coord.value());
    // return;
    // }
    if (input.touch_event.value() == eng::touch::finger_up) {
        // if (!finger_down_pos.has_value())
        // return;
        move_piece(direction_from_displacement(piece_center(*current_piece),
                                               input.f_coord.value()));
        finger_down_pos.reset();
        return;
    }
}

void tetris::game::handle_key(const eng::input_data& input) {
    switch (input.key_event.value()) {
        case eng::event::esc_pressed:
            lost = true;
            break;
        case eng::event::w_pressed:
            move_piece(directions::rotate);
            break;
        case eng::event::s_pressed:
            move_piece(directions::down);
            break;
        case eng::event::d_pressed:
            move_piece(directions::right);
            break;
        case eng::event::a_pressed:
            move_piece(directions::left);
            break;
        default:
            break;
    }
}

void tetris::game::move_piece(const directions& e) {

    switch (e) {
        case directions::rotate:
            if (current_piece) {
                auto tiles{ current_piece->coords_after_rotation() };
                for (const auto& tile : tiles) {
                    if (!within_filed(tile.first, tile.second))
                        goto exit;
                    if (get_tile(tile.first, tile.second) != nullptr)
                        goto exit;
                }
                sounds["rotate"]->play(eng::sound_buffer::properties::once);
                current_piece->rotate();
            }
            break;
        case directions::down:
            if (current_piece) {
                if (movable(current_piece, { 0, -1 })) {
                    current_piece->move(eng::event::s_pressed);
                }
            }
            break;
        case directions::right:
            if (current_piece) {
                if (current_piece) {
                    if (movable(current_piece, { 1, 0 })) {
                        current_piece->move(eng::event::d_pressed);
                    }
                }
                break;
                case directions::left:
                    if (current_piece) {
                        if (movable(current_piece, { -1, 0 })) {
                            current_piece->move(eng::event::a_pressed);
                        }
                    }
                    break;
            }
        default:
        exit:
            break;
    }
}