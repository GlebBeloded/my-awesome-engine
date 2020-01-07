#include "tetris.hpp"
#include "pieces.hpp"
#include "state.hpp"
#include "types.hpp"
#include <random>

tetris::game::game(std::random_device& seed, eng::engine* e)
    : random_engine{}
    , engine{ e } {
    random_engine.seed();
}

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

    render_grid();
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
        if (p.coords.second > 20)
            lost = true;
    }
}

inline bool tetris::game::is_free(int x, int y) {
    return field[x * y] == nullptr;
}
void tetris::game::play() {
    while (!lost) {
        round();
        eng::event event;
        while (engine->read_input(event)) {
            switch (event) {
                case eng::event::esc_pressed:
                    lost = true;
                    break;
                case eng::event::w_pressed:
                    current_piece->rotate();
                    break;
                case eng::event::s_pressed:
                    current_piece->move(event);
                    break;
                case eng::event::d_pressed:
                    current_piece->move(event);
                    break;
                case eng::event::a_pressed:
                    current_piece->move(event);
                    break;
                default:
                    break;
            }
        }
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
    // current_piece->move(eng::event::s_pressed);
}
