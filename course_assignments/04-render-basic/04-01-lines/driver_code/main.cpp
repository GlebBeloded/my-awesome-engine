#include "bresenham.hpp"
#include "ppm.hpp"
#include "random.hpp"

int main() {

    constexpr draw_utils::color red{ 255, 0, 0 };
    constexpr draw_utils::color black{ 0, 0, 0 };
    constexpr auto              image_name = "04-01-lines.ppm";

    ppm::image test{ 1920, 1080 };

    std::fill(test.begin(), test.end(), black);

    if (!test.write_image(image_name).good()) {
        throw std::runtime_error("Error writing file to the system");
    }

    if (!test.load_image(image_name)) {
        throw std::runtime_error("Error loading image");
    }

    for (auto i = 0; i < 100; i++)
        bresenham::draw_line(random_point(test.get_x_size(), test.get_y_size()),
                             random_point(test.get_x_size(), test.get_y_size()),
                             random_color(), test);

    test.write_image(image_name);

    return EXIT_SUCCESS;
}