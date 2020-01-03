#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine.hpp"
#include "shaders.hpp"

#include "glad.h"
#include <SDL.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

template <typename T>
static void load_gl_func(const char* func_name, T& result) {
    void* gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer) {
        throw std::runtime_error(std::string("can't load GL function") +
                                 func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
}

void gl_error_check() {

    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
                break;
        }
        assert(false);
    }
}

namespace eng {

template <typename T>
T lerp(T a, T b, float t) {
    return a + t * (b - a);
}

constexpr static std::array<std::string_view, 17> event_names = {
    { /// input events
      "w_pressed", "a_pressed", "s_pressed", "d_pressed", "esc_pressed",
      "w_released", "a_released", "s_released", "d_released", "esc_released" }
};

std::ostream& operator<<(std::ostream& stream, const event e) {
    std::uint32_t           value = static_cast<std::uint32_t>(e);
    constexpr std::uint32_t minimal =
        static_cast<std::uint32_t>(event::w_pressed);
    constexpr std::uint32_t maximal =
        static_cast<std::uint32_t>(event::esc_released);
    if (value >= minimal && value <= maximal) {
        stream << event_names[value];
        return stream;
    } else {
        throw std::runtime_error("too big event value");
    }
}

static std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

std::istream& operator>>(std::istream& is, vertex& v) {
    is >> v.coord;

    is >> v.color.r;
    is >> v.color.g;
    is >> v.color.b;

    is >> v.tx;
    is >> v.ty;
    return is;
}

std::istream& operator>>(std::istream& is, triangle& t) {
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

struct binding {
    constexpr binding(SDL_Keycode k, std::string_view s, event pressed,
                      event released)
        : key(k)
        , name(s)
        , event_pressed(pressed)
        , event_released(released) {}

    SDL_Keycode      key;
    std::string_view name;
    event            event_pressed;
    event            event_released;
};

constexpr std::array<binding, 5> keys{
    { { SDLK_w, "up", event::w_pressed, event::w_released },
      { SDLK_a, "left", event::a_pressed, event::a_released },
      { SDLK_s, "down", event::s_pressed, event::s_released },
      { SDLK_d, "right", event::d_pressed, event::d_released },
      { SDLK_ESCAPE, "escape", event::esc_pressed, event::esc_released }

    }
};

static bool check_input(const SDL_Event& e, const binding*& result) {
    using namespace std;

    const auto it = find_if(begin(keys), end(keys), [&](const binding& b) {
        return b.key == e.key.keysym.sym;
    });

    if (it != end(keys)) {
        result = &(*it);
        return true;
    }
    return false;
}

class sdl_engine final : public engine {
public:
    sdl_engine(const std::filesystem::path& game_path);
    virtual ~sdl_engine();
    bool  read_input(event& e) final;
    void  render_line(const line& l) override;
    void  render_triangle(const triangle& t) override;
    void  swap_buffers() override;
    float time_from_init() override;
    uint  load_texture(std::string_view path, int width, int height,
                       int nrChannels, int texture_mode) override;

private:
    SDL_GLContext gl_context = nullptr;
    // map of strings mapped to the program IDs
    std::map<std::string, GLuint> shaders;
    SDL_Window*                   window = nullptr;

    void init_SDL();
};

engine* new_sdl_engine(const std::filesystem::path& game_path) {
    return new sdl_engine(game_path);
}

void sdl_engine::init_SDL() {
    using namespace std;

    SDL_version compiled = { 0, 0, 0 };
    SDL_version linked   = { 0, 0, 0 };

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    if (SDL_COMPILEDVERSION !=
        SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
        std::cerr << "warning: SDL2 compiled and linked version mismatch: "
                  << compiled << " " << linked << endl;
    }

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0) {
        const char* err_message = SDL_GetError();
        std::cerr << "error: failed call SDL_Init: " << err_message << endl;
        throw std::runtime_error("Failed to init SDL");
    }

    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 300, 600,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr) {
        const char* err_message = SDL_GetError();
        std::cerr << "error: failed call SDL_CreateWindow: " << err_message
                  << endl;
        SDL_Quit();
        throw std::runtime_error("Failed to create a window");
    }

    int gl_major_ver = 2;
    int gl_minor_ver = 1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        gl_context = SDL_GL_CreateContext(window);
    }

    assert(gl_context != nullptr);

    int result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
    assert(result == 0);

    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
    assert(result == 0);

    if (gl_major_ver <= 2 && gl_minor_ver < 1) {
        std::clog << "current context opengl version: " << gl_major_ver << '.'
                  << gl_minor_ver << '\n'
                  << "need opengl version at least: 2.1\n"
                  << std::flush;
        throw std::runtime_error("opengl version too low");
    }
}

sdl_engine::sdl_engine(const std::filesystem::path& game_path) {
    using namespace std;

    // Create SDL window with opengl context
    init_SDL();

    // Get opengl functions from GLAD
    if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0) {
        throw std::logic_error("error: failed to initialize glad");
    }
    // create vertex and fragment shader for drawning triangles
    const std::vector<std::string_view> triangle_attributes{ "a_position",
                                                             "a_color",
                                                             "a_tex_coord" };

    auto shaders_dir{ game_path / "shaders" };

    shaders["triangle"] = create_shader_program(shaders_dir / "triangle.vert",
                                                shaders_dir / "triangle.frag",
                                                triangle_attributes);

    gl_error_check();
    if (0 == shaders["triangle"]) {
        throw std::runtime_error("failed to create gl program");
    }

    glValidateProgram(shaders["triangle"]);
    gl_error_check();

    // create vertex and fragment shader for drawning lines
    const std::vector<std::string_view> line_attributes{ "a_position" };

    shaders["line"] = create_shader_program(
        shaders_dir / "line.vert", shaders_dir / "line.frag", line_attributes);

    gl_error_check();
    if (0 == shaders["line"]) {
        throw std::runtime_error("failed to create gl program");
    }

    glValidateProgram(shaders["line"]);
    gl_error_check();

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    gl_error_check();
}

sdl_engine::~sdl_engine() {
    if (this == nullptr) {
        throw std::runtime_error("engine is a nullpointer");
    }
    SDL_GL_DeleteContext(gl_context);
}

/// pool event from input queue
/// return true if more events in queue
bool sdl_engine::read_input(event& e) {
    using namespace std;
    // collect all events from SDL
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event)) {
        const binding* binding = nullptr;

        if (sdl_event.type == SDL_QUIT) {
            e = event::esc_pressed;
            return true;
        } else if (sdl_event.type == SDL_KEYDOWN) {
            if (check_input(sdl_event, binding)) {
                e = binding->event_pressed;
                return true;
            }
        } else if (sdl_event.type == SDL_KEYUP) {
            if (check_input(sdl_event, binding)) {
                e = binding->event_released;
                return true;
            }
        }
    }
    return false;
}

void sdl_engine::render_triangle(const triangle& t) {
    auto program_id = shaders["triangle"];
    glUseProgram(program_id);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), &t.v[0]);
    gl_error_check();

    glEnableVertexAttribArray(0);
    gl_error_check();

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          &t.v[0].color.r);
    gl_error_check();

    glEnableVertexAttribArray(1);
    gl_error_check();

    glValidateProgram(program_id);
    gl_error_check();

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &t.v[0].tx);
    gl_error_check();

    glEnableVertexAttribArray(2);
    gl_error_check();

    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &validate_status);
    gl_error_check();
    if (validate_status == GL_FALSE) {
        GLint infoLen = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id, infoLen, nullptr, infoLog.data());
        gl_error_check();
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    gl_error_check();
}

void sdl_engine::render_line(const line& l) {
    auto program_id = shaders["line"];
    glUseProgram(program_id);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, &l);

    glEnableVertexAttribArray(0);
    gl_error_check();

    glValidateProgram(program_id);
    gl_error_check();

    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &validate_status);
    gl_error_check();
    if (validate_status == GL_FALSE) {
        GLint infoLen = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id, infoLen, nullptr, infoLog.data());
        gl_error_check();
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }

    glDrawArrays(GL_LINES, 0, 2);
    gl_error_check();
}

void sdl_engine::swap_buffers() {
    SDL_GL_SwapWindow(window);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl_error_check();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_error_check();
}

vertex blend(const vertex& a, const vertex& b, const float t) {
    vertex r;
    r.coord.x = lerp(a.coord.x, b.coord.x, t);
    r.coord.y = lerp(a.coord.y, b.coord.y, t);
    r.coord.z = lerp(a.coord.z, b.coord.z, t);
    r.color.r = lerp(a.color.r, b.color.r, t);
    r.color.g = lerp(a.color.g, b.color.g, t);
    r.color.b = lerp(a.color.b, b.color.b, t);
    r.tx      = lerp(a.tx, b.tx, t);
    r.ty      = lerp(a.ty, b.ty, t);

    return r;
}

triangle blend(const triangle& tl, const triangle& tr, const float a) {
    triangle r;
    r.v[0] = blend(tl.v[0], tr.v[0], a);
    r.v[1] = blend(tl.v[1], tr.v[1], a);
    r.v[2] = blend(tl.v[2], tr.v[2], a);
    return r;
}

float sdl_engine::time_from_init() {
    std::uint32_t ms_from_library_initialization = SDL_GetTicks();
    float         seconds = ms_from_library_initialization * 0.001f;
    return seconds;
}

uint sdl_engine::load_texture(std::string_view path, int width, int height,
                              int nrChannels, int texture_mode) {
    unsigned char* data =
        stbi_load(path.data(), &width, &height, &nrChannels, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    gl_error_check();

    glBindTexture(GL_TEXTURE_2D, texture);
    gl_error_check();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, texture_mode,
                 GL_UNSIGNED_BYTE, data);
    gl_error_check();

    glGenerateMipmap(GL_TEXTURE_2D);
    gl_error_check();

    stbi_image_free(data);

    return texture;
}
} // namespace eng
