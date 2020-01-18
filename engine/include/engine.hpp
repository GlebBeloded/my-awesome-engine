#pragma once

#include "matrix.hpp"
#include <iosfwd>
#include <memory>
#include <optional>
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

enum class event_type { key, touch };
enum class touch { finger_up, finger_down };

struct input_data {
    event_type                             eventType;
    std::optional<event>                   key_event{};
    std::optional<touch>                   touch_event{};
    std::optional<std::pair<float, float>> f_coord{};
};

struct rgb {
    float r       = 0.f;
    float g       = 0.f;
    float b       = 0.f;
    rgb&  operator=(const rgb& rh);
};

struct vertex {
    matrix::vector coord;
    rgb            color;
    float          tx = 0.f;
    float          ty = 0.f;
};

vertex blend(const vertex& vl, const vertex& vr, const float a);

struct triangle {
    vertex v[3];
};

triangle blend(const triangle& tl, const triangle& tr, const float a);

struct line {
    matrix::vector a;
    matrix::vector b;
};

std::istream& operator>>(std::istream& is, vertex&);
std::istream& operator>>(std::istream& is, triangle&);

std::ostream& operator<<(std::ostream& stream, const event e);

class sound_buffer {
public:
    enum class properties { once, looped };
    virtual ~sound_buffer();
    virtual void play(const properties) = 0;
};

// type[touch,key] handle event
// if key
// if touch
// we need to lock
class engine {
public:
    /// pool event from input queue
    /// return true if more events in queue
    virtual bool  read_input(input_data&)                        = 0;
    virtual void  render_triangle(const triangle&)               = 0;
    virtual void  render_line(const line& l)                     = 0;
    virtual void  swap_buffers()                                 = 0;
    virtual float time_from_init()                               = 0;
    virtual uint  load_texture(std::string_view path, int width, int height,
                               int nrChannels, int texture_mode) = 0;

    virtual sound_buffer* create_sound_buffer(std::string_view path) = 0;
    virtual void          destroy_sound_buffer(sound_buffer*)        = 0;

    virtual ~engine(){};
};

engine*  new_sdl_engine(const std::string_view& assets_path);
triangle blend(const triangle& tl, const triangle& tr, const float a);

struct membuf : public std::streambuf {
    membuf()
        : std::streambuf()
        , buf()
        , buf_size(0) {}
    membuf(std::unique_ptr<char[]> buffer, std::size_t size)
        : std::streambuf()
        , buf(std::move(buffer))
        , buf_size(size) {
        char* beg_ptr = buf.get();
        char* end_ptr = beg_ptr + buf_size;
        setg(beg_ptr, beg_ptr, end_ptr);
        setp(beg_ptr, end_ptr);
    }
    membuf(membuf&& other) {
        setp(nullptr, nullptr);
        setg(nullptr, nullptr, nullptr);

        other.swap(*this);

        buf      = std::move(other.buf);
        buf_size = other.buf_size;

        other.buf_size = 0;
    }

    pos_type seekoff(off_type pos, std::ios_base::seekdir seek_dir,
                     std::ios_base::openmode) override {
        // TODO implement it in correct way
        if (seek_dir == std::ios_base::beg) {
            return 0 + pos;
        } else if (seek_dir == std::ios_base::end) {
            return buf_size + pos;
        } else {
            return egptr() - gptr();
        }
    }

    char*  begin() const { return eback(); }
    size_t size() const { return buf_size; }

private:
    std::unique_ptr<char[]> buf;
    std::size_t             buf_size;
};

membuf load_file(std::string_view path);

} // namespace eng