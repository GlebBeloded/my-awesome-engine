#include "bresenham.hpp"
#include "ppm.hpp"

int main() {

    constexpr draw_utils::color red{ 255, 0, 0 };
    constexpr draw_utils::color black{ 0, 0, 0 };

    ppm::image test{ 320, 240 };

    std::fill(test.begin(), test.end(), black);

    if (!test.write_image("image.ppm").good()) {
        throw std::runtime_error("Error writing file to the system");
    }

    if (!test.load_image("image.ppm")) {
        throw std::runtime_error("Error loading image");
    }

    bresenham::draw_line(0, 0, 319, 239, red, test);

    bresenham::draw_line(0, 200, 50, 0, red, test);

    test.write_image("image.ppm");

    return EXIT_SUCCESS;
}