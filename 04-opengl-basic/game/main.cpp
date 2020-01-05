#include "engine.hpp"
#include "glad.h"
#include "pieces.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

void render_grid(eng::engine* eng);

int main(int /*argc*/, char* argv[]) {

    auto game_dir = std::filesystem::absolute(argv[0]);
    game_dir.remove_filename();

    // create engine
    std::unique_ptr<eng::engine> engine(eng::new_sdl_engine(game_dir));

    // load texture
    engine->load_texture((game_dir / "textures" / "square.png").string(), 256,
                         256, 4, GL_LUMINANCE);

    eng::triangle square[4];
    std::ifstream file;
    file.exceptions(std::ios_base::failbit);

    file.open((game_dir / "textures" / "square.coord").string());

    file >> square[0];
    file >> square[1];
    file.close();

    file.open((game_dir / "textures" / "square.coord").string());
    file >> square[2];
    file >> square[3];

    matrix::vector test;
    // we need to move x by tile.x length, x length = 2.f / 10.f
    // somehow 1 is correct
    test.x = 1;

    for (auto& vertex : square[2].v) {
        vertex.coord = vertex.coord * matrix::move(test);
    }
    for (auto& vertex : square[3].v) {
        vertex.coord = vertex.coord * matrix::move(test);
    }

    matrix::vector move;
    // clang-format off
    //for x step is 0.2 if field width is 10, and square is rendered by it's centerpiece, so we have to adjust by dividing 0.2 further by 2 and getting 0.1
    // this way x coordinate is (-1 + ( (2 / WIDTH_IN_SQUARES) * DISPLACEMENT_TO_THE_RIGHT ) - (2 / WIDTH_IN_SQUARES / 2) )
    //                          -1 is start of coordinates                                   2 == [-1,1]   2 / WIDTH_IN_SQUARES is square size in x dimension
    //                                                                                       since the result is position between two tiles and square would be rendered on the line between those two tiles
    //                                                                                       we substract half of the one displacement in the given dimension to further get desired coordinates, i.e:
    //COORDIANTE_SPACE_MIN + (COORDINATE_SPACE = abs(COORDIANTE_SPACE_MIN)+abs(COORDIANTE_SPACE_MAX) / SQUARES_IN_DIMENSION) * POSITIVE_DISPLACEMENT - (COORDINATE_SPACE / SQUARES_IN_DIMENSION / 2)
    // clang-format on
    move.x = -1.f + (2.f / 10.f) * 1 - (2.f / 10.f / 2.f);
    move.y = -1.f + (2.f / 20.f) * 1 - (2.f / 20.f / 2.f);

    move.x = 0 - (2.f / 10.f / 2.f);
    move.y = 0 - (2.f / 20.f / 2.f);

    for (auto& triangle : square) {
        for (auto& vertex : triangle.v) {
            vertex.coord = vertex.coord * matrix::scale(0.2f, 0.1f);
        }
    }

    tetris::O test_piece{};

    // figure out why you need two loops
    bool continue_loop = true;
    while (continue_loop) {
        eng::event event;

        while (engine->read_input(event)) {
            std::cout << event << std::endl;
            switch (event) {
                case eng::event::esc_pressed:
                    continue_loop = false;
                    break;
                case eng::event::w_pressed:
                    test_piece.rotate();
                    break;
                case eng::event::s_pressed:
                    test_piece.move(event);
                    break;
                case eng::event::d_pressed:
                    test_piece.move(event);
                    break;
                case eng::event::a_pressed:
                    test_piece.move(event);
                    break;
                default:
                    break;
            }
        }

        float alpha = (std::sin(engine->time_from_init()) * 0.5f) + 0.5f;

        auto tr_res = eng::blend(square[0], square[1], alpha);

        // engine->render_triangle(square[0]);
        // engine->render_triangle(square[1]);
        // engine->render_triangle(square[2]);
        // engine->render_triangle(square[3]);
        test_piece.render(engine.get());

        render_grid(engine.get());

        engine->swap_buffers();
    }

    return EXIT_SUCCESS;
}

void render_grid(eng::engine* eng) {
    float horizontal_step = 2.f / 20.f;
    float vertical_step   = 2.f / 10.f;

    for (float i = -1.f; i <= 1.f; i += horizontal_step) {
        eng::line line;
        line.a.x = -1;
        line.a.y = i;

        line.b.x = 1;
        line.b.y = i;

        eng->render_line(line);
    }

    for (float i = -1.f; i <= 1.f; i += vertical_step) {
        eng::line line;
        line.a.x = i;
        line.a.y = -1;

        line.b.x = i;
        line.b.y = 1;

        eng->render_line(line);
    }
}