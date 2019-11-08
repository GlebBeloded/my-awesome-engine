#include "engine.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>

int main(int /*argc*/, char* /*argv*/ []) {
    std::unique_ptr<custom_engine::engine> engine(
        custom_engine::new_sdl_engine());

    // figure out why you need two loops
    bool continue_loop = true;
    while (continue_loop) {
        custom_engine::event event;

        while (engine->read_input(event)) {
            std::cout << event << std::endl;
            switch (event) {
                case custom_engine::event::esc_pressed:
                    continue_loop = false;
                    break;
                default:
                    break;
            }
        }
    }

    return EXIT_SUCCESS;
}