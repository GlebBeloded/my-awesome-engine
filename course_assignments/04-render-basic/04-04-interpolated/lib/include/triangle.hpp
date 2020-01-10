#pragma once

#include "ppm.hpp"
#include <cmath>

struct vertex {
    double f0 = 0; /// x
    double f1 = 0; /// y
    double f2 = 0; /// r
    double f3 = 0; /// g
    double f4 = 0; /// b
    double f5 = 0; /// u (texture coordinate)
    double f6 = 0; /// v (texture coordinate)
    double f7 = 0; /// ?
};

struct uniforms {
    double f0 = 0;
    double f1 = 0;
    double f2 = 0;
    double f3 = 0;
    double f4 = 0;
    double f5 = 0;
    double f6 = 0;
    double f7 = 0;
};

struct gfx_program {
    virtual ~gfx_program()                                        = default;
    virtual void              set_uniforms(const uniforms&)       = 0;
    virtual vertex            vertex_shader(const vertex& v_in)   = 0;
    virtual draw_utils::color fragment_shader(const vertex& v_in) = 0;
};

class tri_render : public ppm::image {
    std::vector<vertex> rasterize_line(const vertex& left, const vertex& right);

    std::vector<vertex> rasterize_triangle(const vertex& a, const vertex& b,
                                           const vertex& c);

    std::vector<vertex> rasterize(const vertex& a, const vertex& b,
                                  const vertex& c);
    gfx_program*        program_ = nullptr;

public:
    tri_render(int32_t x, int32_t y, gfx_program* pr)
        : ppm::image{ x, y }
        , program_{ pr } {}

    void draw_triangles(const std::vector<vertex>&   vertexes,
                        const std::vector<uint16_t>& indexes);
};
