#include "engine.hpp"
#include "glad.h"
#include "tetris.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string_view>

void render_grid(eng::engine* eng);

int main(int /*argc*/, char* argv[]) {

    auto game_dir = std::filesystem::absolute(argv[0]);
    game_dir.remove_filename();

    // create engine
    std::unique_ptr<eng::engine> engine(eng::new_sdl_engine(game_dir));

    std::random_device rdevice{};
    tetris::game       tetris{ rdevice, engine.get(), game_dir };

    tetris.play();

    // clang-format off
    //for x step is 0.2 if field width is 10, and square is rendered by it's centerpiece, so we have to adjust by dividing 0.2 further by 2 and getting 0.1
    // this way x coordinate is (-1 + ( (2 / WIDTH_IN_SQUARES) * DISPLACEMENT_TO_THE_RIGHT ) - (2 / WIDTH_IN_SQUARES / 2) )
    //                          -1 is start of coordinates                                   2 == [-1,1]   2 / WIDTH_IN_SQUARES is square size in x dimension
    //                                                                                       since the result is position between two tiles and square would be rendered on the line between those two tiles
    //                                                                                       we substract half of the one displacement in the given dimension to further get desired coordinates, i.e:
    //COORDIANTE_SPACE_MIN + (COORDINATE_SPACE = abs(COORDIANTE_SPACE_MIN)+abs(COORDIANTE_SPACE_MAX) / SQUARES_IN_DIMENSION) * POSITIVE_DISPLACEMENT - (COORDINATE_SPACE / SQUARES_IN_DIMENSION / 2)
    // clang-format on

    return EXIT_SUCCESS;
}
