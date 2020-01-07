#include "state.hpp"
#include "pieces.hpp"
#include "types.hpp"
#include <random>

namespace tetris {
class game {
public:
    game(std::random_device&, eng::engine*);
    void   render_board();
    piece* generate_piece();
    void   play();
    void   round();

private:
    float                              speed{ 1.0 };
    bool                               lost{ false };
    inline bool                        is_free(int, int);
    void                               fixate(piece*);
    eng::engine*                       engine;
    std::mt19937                       random_engine;
    piece*                             current_piece{ nullptr };
    std::uniform_int_distribution<int> pieces_distribution{ 0, 6 };
    std::uniform_int_distribution<int> color_distribution{ 0,
                                                           colors.size() - 1 };
    std::array<tile*, state::board_size.x * state::board_size.y> field{};
};
} // namespace tetris

tetris::game::game(std::random_device& seed, eng::engine* e)
    : random_engine{}
    , engine{ e } {
    random_engine.seed();
}

void tetris::game::render_board() {
    for (const auto& tile : field)
        if (tile != nullptr)
            tile->render(engine);
    if (this->current_piece != nullptr)
        current_piece->render(engine);
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
        field[p.coords.first * p.coords.second] = new tile{ p };
    }
}

inline bool tetris::game::is_free(int x, int y) {
    return field[x * y] == nullptr;
}
void tetris::game::play() {
    while (!lost) {
        round();
        eng::event event;
        engine->read_input(event);
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
        render_board();
    }
}

void tetris::game::round() {
    if (current_piece == nullptr)
        current_piece = generate_piece();

    for (const auto& tile : current_piece->get_tiles()) {
        if ((field.at(tile.coords.first - 1 * tile.coords.second) != nullptr) ||
            tile.coords.first == 1) {
            fixate(current_piece);
        }
    }
}