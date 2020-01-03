#include "engine.hpp"
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

    //create engine
    std::unique_ptr<eng::engine> engine(eng::new_sdl_engine(game_dir));

    //load texture
    engine->load_texture((game_dir / "textures" / "square.png").string(),256, 256, 4,0x1909);

    eng::triangle square[2];
    std::ifstream file;
    file.exceptions(std::ios_base::failbit);

    file.open((game_dir / "textures" / "square.coord").string());


    file >> square[0];
    file >> square[1];

    matrix::vector move;
    move.x = 0.1;
    move.y = 0.05;

    for (auto& triangle : square){
        for(auto& vertex: triangle.v){
            vertex.coord = vertex.coord * matrix::scale(0.2f,0.1f) * matrix::move(move);
        }
    }
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
                default:
                    break;
            }
        }

        float alpha = (std::sin(engine->time_from_init()) * 0.5f) + 0.5f;

        auto tr_res = eng::blend(square[0], square[1], alpha);

        engine->render_triangle(square[0]);
        engine->render_triangle(square[1]);

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