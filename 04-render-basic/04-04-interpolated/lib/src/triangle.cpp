#include "triangle.hpp"
#include <iostream>

double interpolate(const double f0, const double f1, const double t) {
    if (t < 0.0 || t > 1.0)
        throw std::logic_error("invalid interpolation interval");
    return f0 + (f1 - f0) * t;
}
// to interpolate a vertex means to means to interpolate each of it's members
vertex interpolate(const vertex& v0, const vertex& v1, const double t) {
    return { interpolate(v0.f0, v1.f0, t), interpolate(v0.f1, v1.f1, t),
             interpolate(v0.f2, v1.f2, t), interpolate(v0.f3, v1.f3, t),
             interpolate(v0.f4, v1.f4, t), interpolate(v0.f5, v1.f5, t),
             interpolate(v0.f6, v1.f6, t), interpolate(v0.f7, v1.f7, t) };
}

std::vector<vertex> tri_render::rasterize_line(const vertex& left,
                                               const vertex& right) {

    std::vector<vertex> positions;
    size_t              num_of_pixels_in_line =
        static_cast<size_t>(std::round(std::abs(left.f0 - right.f0)));
    if (num_of_pixels_in_line > 0) {
        //  removed +1 from for loop
        for (size_t p = 0; p <= num_of_pixels_in_line; ++p) {
            double t_pixel =
                static_cast<double>(p) / (num_of_pixels_in_line + 1);
            vertex pixel = interpolate(left, right, t_pixel);

            positions.push_back(pixel);
        }
    } else {
        positions.push_back(left);
    }
    return positions;
}

std::vector<vertex> tri_render::rasterize_triangle(const vertex& a,
                                                   const vertex& b,
                                                   const vertex& c) {
    std::vector<vertex> verticies;

    size_t num_of_hlines =
        static_cast<size_t>(std::round(std::abs(a.f1 - b.f1)));

    if (num_of_hlines > 0) {
        for (size_t i = 0; i <= num_of_hlines; ++i) {
            double t_vertical   = static_cast<double>(i) / num_of_hlines;
            vertex left_vertex  = interpolate(b, a, t_vertical);
            vertex right_vertex = interpolate(c, a, t_vertical);
            auto   line         = rasterize_line(left_vertex, right_vertex);
            verticies.insert(verticies.end(), line.begin(), line.end());
        }
    } else {
        auto line = rasterize_line(b, c);
        verticies.insert(verticies.end(), line.begin(), line.end());
    }

    return verticies;
}

std::vector<vertex> tri_render::rasterize(const vertex& a, const vertex& b,
                                          const vertex& c) {

    std::array<const vertex*, 3> in_vertexes{ &a, &b, &c };
    std::sort(std::begin(in_vertexes), std::end(in_vertexes),
              [](const vertex* left, const vertex* right) {
                  return left->f1 < right->f1;
              });

    const vertex& top    = *in_vertexes.at(0);
    const vertex& middle = *in_vertexes.at(1);
    const vertex& bottom = *in_vertexes.at(2);

    if (top.f1 == bottom.f1) {
        return rasterize_line(top, bottom);
    }

    else if (middle.f1 == bottom.f1) { // top triangle
        return rasterize_triangle(top, middle, bottom);
    }

    else if (top.f1 ==
             middle.f1) { // bottom triangle  //swap top and bottom parts
        return rasterize_triangle(bottom, middle, top);
    }

    std::vector<vertex> verticies;

    vertex divider = { (top.f0 + ((middle.f1 - top.f1) / (bottom.f1 - top.f1)) *
                                     (bottom.f0 - top.f0)),
                       middle.f1 };

    auto bottom_tri = rasterize_triangle(top, middle, divider);
    auto top_tri    = rasterize_triangle(bottom, middle, divider);

    verticies.insert(verticies.end(), top_tri.begin(), top_tri.end());
    verticies.insert(verticies.end(), bottom_tri.begin(), bottom_tri.end());

    return verticies;
}

void tri_render::draw_triangles(const std::vector<vertex>&   vertexes,
                                const std::vector<uint16_t>& indexes) {
    for (size_t index = 0; index < indexes.size(); index += 3) {
        const uint16_t index0 = indexes.at(index + 0);
        const uint16_t index1 = indexes.at(index + 1);
        const uint16_t index2 = indexes.at(index + 2);

        const vertex& v0 = vertexes.at(index0);
        const vertex& v1 = vertexes.at(index1);
        const vertex& v2 = vertexes.at(index2);

        const vertex v0_ = program_->vertex_shader(v0);
        const vertex v1_ = program_->vertex_shader(v1);
        const vertex v2_ = program_->vertex_shader(v2);

        const std::vector<vertex> interpoleted{ rasterize(v0_, v1_, v2_) };
        for (const vertex& interpolated_vertex : interpoleted) {
            const draw_utils::color c =
                program_->fragment_shader(interpolated_vertex);

            const draw_utils::point pos{
                static_cast<int32_t>(std::round(interpolated_vertex.f0)),
                static_cast<int32_t>(std::round(interpolated_vertex.f1))
            };
            try {
                at(pos) = c;
            } catch (const std::exception& e) {
                std::cerr << "trying to draw outside of image" << std::endl;
            }
        }
    }
}