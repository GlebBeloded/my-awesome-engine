#include "engine.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

int main(int /*argc*/, char* /*argv*/[]) {
    std::unique_ptr<eng::engine> engine(eng::new_sdl_engine());

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
        std::ifstream file;
        file.open("/home/gleb/projects/my-awesome-engine/build/04-opengl-basic/"
                  "vertexes.txt");
        file.exceptions(std::ios_base::failbit);

        eng::triangle tr;

        file >> tr;
        engine->render_triangle(tr);

        engine->swap_buffers();
    }

    return EXIT_SUCCESS;
}