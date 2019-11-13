#include "renderer.hpp"
#include "triangle_utils.hpp"

int main() {

    triangle::renderer rend(320, 240);

    auto [vertexes, indexes] =
        triangle_net(10, 10, rend.get_x_size(), rend.get_y_size());

    rend.draw(vertexes, indexes, { 255, 0, 0 });

    if (!rend.write_image("vertex.ppm")) {
        throw std::runtime_error("error writing file to disk");
    }
    return EXIT_SUCCESS;
}