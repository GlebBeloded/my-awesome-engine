#include "tetris.hpp"
#include "pieces.hpp"
#include "state.hpp"
#include "types.hpp"
#include <random>

float normalize(const float& min, const float& max,
                const float& current_value) {
    return (current_value - min) / (max - min);
}

int get_field_index(int x, int y) {
    return x + (y * state::board_size.first);
}

void tetris::render_state::render(eng::engine* e, float time) {
    float alpha = normalize(begin_time, end_time, time);
    if (alpha > 1.0) {
        alpha    = 1.0;
        finished = true;
    }
    for (const auto& [key, value] : full_state) {

        if (rows_to_delete[key])
            continue;
        if (key < threshhold_value)
            value.render(e);
        if (key >= threshhold_value) {
            auto moved{ value };
            moved.move_to_coords(value.coords.first,
                                 value.coords.second - rows_destroyed);
            lerp(value, moved, alpha).render(e);
        }
    }
}

void tetris::render_state::initialize(float b, float e) {
    if (initialized == true)
        throw std::logic_error("render process already taking place");
    begin_time  = b;
    end_time    = e;
    initialized = true;
    finished    = false;
}

void tetris::render_state::reset() {
    initialized = false;
    for (const auto& [key, val] : rows_to_delete) {
        if (val)
            full_state.erase(key);
    }
    // move all tyles n rows lower
    std::vector<int> invalid_indexes;
    for (auto& [key, value] : full_state) {
        if (key >= threshhold_value) {
            value.move_to_coords(value.coords.first,
                                 value.coords.second - rows_destroyed);
            full_state[get_field_index(value.coords.first,
                                       value.coords.second)] = value;
            invalid_indexes.push_back(key);
        }
    }
    // for (const auto& index : invalid_indexes) {
    // full_state.erase(index);
    // }

    threshhold_value = state::board_size.first * (state::board_size.second + 5);
    finished         = false;
    rows_destroyed   = 0;

    rows_to_delete.clear();
}

tetris::game::game(std::random_device& seed, eng::engine* e)
    : random_engine{ seed() }
    , engine{ e } {}

void tetris::game::render_grid() {

    float vertical_step   = 2.f / static_cast<float>(state::board_size.first);
    float horizontal_step = 2.f / static_cast<float>(state::board_size.second);

    for (float i = -1.f; i <= 1.f; i += horizontal_step) {
        eng::line line;
        line.a.x = -1;
        line.a.y = i;

        line.b.x = 1;
        line.b.y = i;

        engine->render_line(line);
    }

    for (float i = -1.f; i <= 1.f; i += vertical_step) {
        eng::line line;
        line.a.x = i;
        line.a.y = -1;

        line.b.x = i;
        line.b.y = 1;

        engine->render_line(line);
    }
}

void tetris::game::render_board() {
    for (auto tile : field)
        if (tile != nullptr)
            tile->render(engine);
    if (this->current_piece != nullptr)
        current_piece->render(engine);

    // render_grid();
}

// returns RAW POINTER
tetris::piece* tetris::game::generate_piece() {
    auto type = static_cast<piece_types>(pieces_distribution(random_engine));

    auto   col = static_cast<color>(color_distribution(random_engine));
    piece* ptr;

    switch (type) {
        case piece_types::O:
            ptr = new O{};
            break;
        case piece_types::I:
            ptr = new I{};
            break;
        case piece_types::J:
            ptr = new J{};
            break;
        case piece_types::L:
            ptr = new L{};
            break;
        case piece_types::T:
            ptr = new T{};
            break;
        case piece_types::S:
            ptr = new S{};
            break;
        case piece_types::Z:
            ptr = new Z{};
            break;
        default:
            throw std::logic_error("Invalid enum class value");
    }
    ptr->set_color(col);
    return ptr;
}

void tetris::game::fixate(tetris::piece* ptr) {
    for (auto p : ptr->get_tiles()) {
        field[(p.coords.first + (state::board_size.first * p.coords.second))] =
            new tile{ p };
        rstate.full_state[get_field_index(p.coords.first, p.coords.second)] = p;
        if (p.coords.second > state::board_size.second)
            lost = true;
    }
}

void tetris::game::play() {
    auto time = engine->time_from_init();
    while (!lost) {
        // round();
        eng::event event;
        while (engine->read_input(event)) {
            switch (event) {
                case eng::event::esc_pressed:
                    lost = true;
                    break;
                case eng::event::w_pressed:
                    if (current_piece) {
                        auto tiles{ current_piece->coords_after_rotation() };
                        for (const auto& tile : tiles) {
                            if (!within_filed(tile.first, tile.second))
                                goto exit;
                            if (get_tile(tile.first, tile.second) != nullptr)
                                goto exit;
                        }
                        current_piece->rotate();
                    }
                    break;
                case eng::event::s_pressed:
                    if (current_piece) {
                        if (movable(current_piece, { 0, -1 }))
                            current_piece->move(event);
                    }
                    break;
                case eng::event::d_pressed:
                    if (current_piece) {
                        if (current_piece) {
                            if (movable(current_piece, { 1, 0 }))
                                current_piece->move(event);
                        }
                        break;
                        case eng::event::a_pressed:
                            if (current_piece) {
                                if (movable(current_piece, { -1, 0 }))
                                    current_piece->move(event);
                            }
                            break;
                    }
                default:
                exit:
                    break;
            }
        }
        if (time + step_time < engine->time_from_init()) {
            time = engine->time_from_init();
            round();
            if (current_piece) {
                if (movable(current_piece, { 0, -1 }))
                    current_piece->move(eng::event::s_pressed);
            }
        }
        clear(0);

        // NEW RENDERER(with bugs)
        // if (rstate.rows_destroyed > 0) {
        //     if (!rstate.initialized) {
        //         rstate.initialize(engine->time_from_init(),
        //                           engine->time_from_init() + step_time);
        //         rstate.render(engine, engine->time_from_init());
        //     }
        //     if (current_piece)
        //         current_piece->render(engine);
        // }
        // if (rstate.rows_destroyed > 0 && rstate.finished)
        //     rstate.reset();

        // if (current_piece)
        //     current_piece->render(engine);
        // rstate.render(engine, engine->time_from_init());
        render_board();
        engine->swap_buffers();
    }
}

void tetris::game::round() {
    if (current_piece == nullptr)
        current_piece = generate_piece();

    for (const auto& tile : current_piece->get_tiles()) {

        if (tile.coords.second == 0) {
            fixate(current_piece);
            delete current_piece;
            current_piece = nullptr;
            return;
        }

        if ((field.at((tile.coords.first +
                       (state::board_size.first * (tile.coords.second - 1)))) !=
             nullptr)) {
            fixate(current_piece);
            delete current_piece;
            current_piece = nullptr;
            return;
        }
    }
}

void tetris::game::shift_down(int bottom_row) {
    // for each row
    for (int i = bottom_row; i < state::board_size.second; i++) {
        //   for each row cell
        for (int x = 0; x < state::board_size.first; x++) {
            // if not empty
            if (field.at(x + (i * state::board_size.first)) != nullptr) {
                // move tile coords
                field.at(x + (i * state::board_size.first))->move_down();
                // change board coords
                field.at(x + ((i - 1) * state::board_size.first)) =
                    new tetris::tile{ *(
                        field.at(x + (i * state::board_size.first))) };
                delete field.at(x + (i * state::board_size.first));
                field.at(x + (i * state::board_size.first)) = nullptr;
            }
        }
    }
}

bool tetris::game::is_full(int row) {
    for (int x = 0; x < state::board_size.first; x++) {
        if (field.at(x + (state::board_size.first * row)) == nullptr)
            return false;
    }
    return true;
}

void tetris::game::clear_row(int row) {
    if (rstate.initialized)
        throw std::logic_error(
            "rows removed before previous animation has finished");
    // lower threshold above which tiles should be interpolated if needed
    if (rstate.threshhold_value > get_field_index(0, row))
        rstate.threshhold_value = get_field_index(0, row);
    // increment -y displacement to render
    rstate.rows_destroyed++;
    for (int x = 0; x < state::board_size.first; x++) {
        rstate.rows_to_delete[get_field_index(x, row)] = true;
        delete (field.at(x + (state::board_size.first * row)));
        field.at(x + (state::board_size.first * row)) = nullptr;
    }
}

// recursive
void tetris::game::clear(int row) {
    for (int i = row; i < state::board_size.second; i++) {
        if (is_full(i)) {
            clear_row(i);
            shift_down(i + 1);
            clear(i);
            return;
        }
    }
}

tetris::tile*& tetris::game::get_tile(int x, int y) {
    return field.at(x + (y * state::board_size.first));
}

bool tetris::game::movable(tetris::piece*&            piece,
                           const std::pair<int, int>& displacement) {
    for (auto& tile : piece->get_tiles()) {
        if (!within_filed(tile.coords.first + displacement.first,
                          tile.coords.second + displacement.second))
            return false;
        if (get_tile(tile.coords.first + displacement.first,
                     tile.coords.second + displacement.second) != nullptr)
            return false;
    }
    return true;
}

namespace tetris {
std::vector<tile> lerp(const std::vector<tile>& a, const std::vector<tile>& b,
                       float alpha) {
    if (a.size() != b.size()) {
        throw std::logic_error("cannot interpolate uneven number of objects");
    }
    std::vector<tile> result(a.size());
    for (auto i = 0; i < a.size(); i++) {
        result[i] = lerp(a[i], b[i], alpha);
    }
    return result;
}

} // namespace tetris