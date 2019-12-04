#pragma once

#include <iosfwd>
#include <string>
#include <string_view>

namespace eng {
enum class event {
    w_pressed,
    a_pressed,
    s_pressed,
    d_pressed,
    esc_pressed,
    w_released,
    a_released,
    s_released,
    d_released,
    esc_released
};

struct vertex {
    float x  = 0.f;
    float y  = 0.f;
    float z  = 0.f;
    float r  = 0.f;
    float g  = 0.f;
    float b  = 0.f;
    float tx = 0.f;
    float ty = 0.f;
};

vertex blend(const vertex& vl, const vertex& vr, const float a);

struct triangle {
    vertex v[3];
};

triangle blend(const triangle& tl, const triangle& tr, const float a);

std::istream& operator>>(std::istream& is, vertex&);
std::istream& operator>>(std::istream& is, triangle&);

std::ostream& operator<<(std::ostream& stream, const event e);

class engine {
public:
    /// pool event from input queue
    /// return true if more events in queue
    virtual bool  read_input(event& e)             = 0;
    virtual void  render_triangle(const triangle&) = 0;
    virtual void  swap_buffers()                   = 0;
    virtual float time_from_init()                 = 0;
    virtual uint  load_texture(std::string_view path, int width, int height,
                               int nrChannels)     = 0;

    virtual ~engine(){};
};

engine* new_sdl_engine();
} // namespace eng