#pragma once
#include "types.hpp"
#include <map>
#include <random>

namespace tetris {

struct render_state {
    float               begin_time{ 0 };
    float               end_time{ 0 };
    bool                initialized{ false };
    bool                finished{ false };
    std::map<int, tile> full_state; // to do animation we just copy given tile
                                    // and substract rows_destroyed y value
                                    // we need threshold value to not
                                    // interpolate all tiles before it

    int rows_destroyed{ 0 };
    int threshhold_value{
        state::board_size.first * (state::board_size.second + 5)
    }; // interpolate everything above given value (boardsize by default)

    std::map<int, bool> rows_to_delete{};

    void render(eng::engine*, float time);
    void initialize(float b, float e);
    void reset();
};

class game {
public:
    game(std::random_device&, eng::engine*);
    void play();

private:
    void   render_grid();
    tile*& get_tile(int x, int y);
    bool   within_filed(int x, int y) {
        return (x >= 0 && x < state::board_size.first) &&
               (y >= 0 && y < state::board_size.second + 5);
    }
    void         render_board();
    piece*       generate_piece();
    void         round();
    bool         movable(piece*&, const std::pair<int, int>& displacement);
    void         fixate(piece*);
    eng::engine* engine;
    std::mt19937 random_engine;
    void         shift_down(int bottom_row);
    void         clear(int row);
    void         clear_row(int row);
    bool         is_full(int row);
    piece*       current_piece{ nullptr };
    std::uniform_int_distribution<int> pieces_distribution{ 0, 6 };
    std::uniform_int_distribution<int> color_distribution{ 0,
                                                           colors.size() - 1 };
    std::array<tile*, state::board_size.first*(state::board_size.second + 5)>
        field{ nullptr };

    bool         lost{ false };
    float        step_time{ 1.0 };
    render_state rstate{};
    int          rows_destroyed{ 0 };
};

std::vector<tile> lerp(const std::vector<tile>& a, const std::vector<tile>& b,
                       float alpha);
} // namespace tetris
