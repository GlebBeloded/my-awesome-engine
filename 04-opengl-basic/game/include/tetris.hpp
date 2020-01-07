#pragma once
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
    void render_grid();

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
    std::array<tile*, state::board_size.first*(state::board_size.second + 5)>
        field{};
};
} // namespace tetris