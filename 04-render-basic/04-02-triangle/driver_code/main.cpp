#include "renderer.hpp"
#include "triangle_utils.hpp"

int main() {
    triangle::renderer tri(640, 480);

    tri.draw_triangle_lines({ 0, 0 }, { 319, 239 }, { 0, 239 }, { 255, 0, 0 });

    tri.clear({ 0, 0, 0 });

    draw_triangle_net(tri, 20, 15, { 0, 255, 0 });

    if (!tri.write_image("04-02-triangle.ppm")) {
        throw std::runtime_error("could not write image");
    }

    return EXIT_SUCCESS;
}