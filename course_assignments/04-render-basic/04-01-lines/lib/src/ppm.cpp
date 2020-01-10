#include "ppm.hpp"

using namespace ppm;

color& image::at(const point& pos) {
    return std::vector<color>::at(get_x_size() * pos.y + pos.x);
}

std::ofstream image::write_image(const std::string& path) {
    std::ofstream output;

    if (this->x_size * this->y_size != this->size()) {
        throw std::logic_error("invalid ppm image proportions");
    }

    output.open(path, std::ios_base::binary);

    output << "P6" << ' ' << this->x_size << ' ' << this->y_size << ' ' << 255
           << '\n';

    std::streamsize buf_size = static_cast<long>(this->size() * sizeof(color));

    output.write(reinterpret_cast<const char*>(this->data()), buf_size);

    return output;
}

std::ifstream image::load_image(const std::string& path) {
    std::ifstream input;
    input.open(path, std::ios_base::binary);

    std::string header;
    size_t      color_model;

    input >> header >> x_size >> y_size >> color_model >>
        std::ws; // eat next whitespace so '\n' gets eaten and we can read bytes
                 // normally

    this->reserve(x_size * y_size);

    input.read(reinterpret_cast<char*>(this->data()),
               sizeof(color) * x_size * y_size);

    if (this->x_size * this->y_size != this->size()) {
        throw std::logic_error("invalid ppm image proportions");
    }

    return input;
}
