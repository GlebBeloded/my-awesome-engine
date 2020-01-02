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

int main(int /*argc*/, char* argv[]) {

    auto game_dir = std::filesystem::absolute(argv[0]);
    game_dir.remove_filename();

    std::unique_ptr<eng::engine> engine(eng::new_sdl_engine(game_dir));

    engine->load_texture("/home/gleb/projects/my-awesome-engine/build/"
                         "04-opengl-basic/cute-dog-vector-png-2.png",
                         780, 720, 4);

    eng::triangle dog[2];
    std::ifstream file;
    file.exceptions(std::ios_base::failbit);

    file.open("/home/gleb/projects/my-awesome-engine/build/04-opengl-basic/"
              "vertexes.txt");

    // matrix::vector left;
    // left.x = 1.f;

    file >> dog[0];
    file >> dog[1];
    // for (auto& tr : dog) {
    //     for (auto& vec : tr.v) {
    //         matrix::matrix m = matrix::scale(.5f) * matrix::move(left);
    //         vec.coord        = vec.coord * m;
    //     }
    // }

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

        auto tr_res = eng::blend(dog[0], dog[1], alpha);

        engine->render_triangle(dog[0]);
        engine->render_triangle(dog[1]);
        eng::line line;
        line.a.x = -1;
        line.a.y = -1;
        line.a.z = 1;

        line.b.x = 1;
        line.b.y = 1;
        line.b.z = 1;

        // engine->render_line(line);
        engine->swap_buffers();
    }

    return EXIT_SUCCESS;
}