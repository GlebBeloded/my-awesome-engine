#pragma once
#include "types.hpp"
#include <random>

namespace tetris {
class game {
public:
    game(std::random_device&, eng::engine*);
    void play();

private:
    void                               render_grid();
    void                               render_board();
    piece*                             generate_piece();
    void                               round();
    float                              step_time{ 1.0 };
    bool                               lost{ false };
    void                               fixate(piece*);
    eng::engine*                       engine;
    std::mt19937                       random_engine;
    void                               shift_down(int bottom_row);
    void                               clear(int row);
    void                               clear_row(int row);
    bool                               is_full(int row);
    piece*                             current_piece{ nullptr };
    std::uniform_int_distribution<int> pieces_distribution{ 0, 6 };
    std::uniform_int_distribution<int> color_distribution{ 0,
                                                           colors.size() - 1 };
    std::array<tile*, state::board_size.first*(state::board_size.second + 5)>
        field{ nullptr };
};
} // namespace tetris