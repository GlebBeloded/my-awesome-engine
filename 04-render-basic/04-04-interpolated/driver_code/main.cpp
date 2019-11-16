#include "triangle.hpp"

constexpr auto width  = 320;
constexpr auto height = 240;

int main() {

    using namespace draw_utils;
    struct program : gfx_program {
        void   set_uniforms(const uniforms&) override {}
        vertex vertex_shader(const vertex& v_in) override {
            vertex out = v_in;

            // rotate
            double alpha = 3.14159 / 6; // 30 degree
            double x     = out.f0;
            double y     = out.f1;
            out.f0       = x * std::cos(alpha) - y * std::sin(alpha);
            out.f1       = x * std::sin(alpha) + y * std::cos(alpha);

            // scale into 3 times
            out.f0 *= 0.3;
            out.f1 *= 0.3;
            // move
            out.f0 += (width / 2);
            out.f1 += (height / 2);

            return out;
        }
        color fragment_shader(const vertex& v_in) override {
            color out;
            out.r = static_cast<uint8_t>(v_in.f2 * 255);
            out.g = static_cast<uint8_t>(v_in.f3 * 255);
            out.b = static_cast<uint8_t>(v_in.f4 * 255);
            return out;
        }
    } program01;

    tri_render x{ width, height, &program01 };

    std::vector<vertex> triangle_v{ { 0, 0, 1, 0, 0, 0, 0, 0 },
                                    { 0, 239, 0, 1, 0, 0, 239, 0 },
                                    { 319, 239, 0, 0, 1, 319, 239, 0 } };

    x.draw_triangles(triangle_v, { 0, 1, 2 });

    return x.write_image("interpolated.ppm") ? EXIT_SUCCESS : EXIT_FAILURE;
}