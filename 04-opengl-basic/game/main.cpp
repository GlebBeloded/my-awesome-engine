#include "engine.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

int main(int /*argc*/, char* /*argv*/[]) {
    std::unique_ptr<eng::engine> engine(eng::new_sdl_engine());
    engine->load_texture("/home/gleb/projects/my-awesome-engine/build/"
                         "04-opengl-basic/cute-dog-vector-png-2.png",
                         780, 720, 4);
    // figure out why you need two loops
    bool          continue_loop = true;
    eng::triangle tr;
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
        std::ifstream file;
        file.exceptions(std::ios_base::failbit);

        file.open("/home/gleb/projects/my-awesome-engine/build/04-opengl-basic/"
                  "vertexes.txt");

        eng::triangle tr_1;
        file >> tr_1;
        eng::triangle tr_2;
        file >> tr_2;

        float alpha = (std::sin(engine->time_from_init()) * 0.5f) + 0.5f;

        auto tr_res = eng::blend(tr_1, tr_2, alpha);

        engine->render_triangle(tr_1);
        engine->render_triangle(tr_2);
        engine->swap_buffers();
    }

    return EXIT_SUCCESS;
}