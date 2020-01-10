#include "random.hpp"
#include "renderer.hpp"
#include "triangle_utils.hpp"

int main() {

    triangle::renderer rend(320, 240);

    auto [verticies, indicies] =
        triangle_net(10, 10, rend.get_x_size(), rend.get_y_size());

    for (auto i = 0; i < indicies.size() - 1; i++)
        rend.draw(verticies, { indicies[i], indicies[i + 1] }, random_color());

    if (!rend.write_image("vertex.ppm")) {
        throw std::runtime_error("error writing file to disk");
    }
    return EXIT_SUCCESS;
}