#include "engine.hpp"
#include "shaders.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "glad.h"
#include <SDL.h>

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

static bool already_exist = false;

constexpr static std::array<std::string_view, 17> event_names = {
    { /// input events
      "w_pressed", "a_pressed", "s_pressed", "d_pressed", "esc_pressed",
      "w_released", "a_released", "s_released", "d_released", "esc_released" }
};

namespace default_shaders {

std::string_view vertex   = R"(
                                    attribute vec3 a_position;
                                    varying vec4 v_position;

                                    void main()
                                    {
                                        v_position = vec4(a_position, 1.0);
                                        gl_Position = v_position;
                                    }
                                    )";
std::string_view fragment = R"(
                      precision mediump float; 
                      void main()
                      {
                        gl_FragColor = vec4(0.0,1.0,1.0,1.0);
                      }
                      )";
} // namespace default_shaders

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
    is >> v.x;
    is >> v.y;
    is >> v.z;

    is >> v.r;
    is >> v.g;
    is >> v.b;
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
    sdl_engine();
    virtual ~sdl_engine();
    bool read_input(event& e) final;
    void render_triangle(const triangle& t) override;
    void swap_buffers() override;

private:
    SDL_GLContext gl_context  = nullptr;
    GLuint        program_id_ = 0;
    SDL_Window*   window      = nullptr;
};

engine* new_sdl_engine() {
    return new sdl_engine();
}

sdl_engine::sdl_engine() {
    using namespace std;

    stringstream serr;

    SDL_version compiled = { 0, 0, 0 };
    SDL_version linked   = { 0, 0, 0 };

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    if (SDL_COMPILEDVERSION !=
        SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
        serr << "warning: SDL2 compiled and linked version mismatch: "
             << compiled << " " << linked << endl;
    }

    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0) {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_Init: " << err_message << endl;
        throw std::runtime_error("Failed to init SDL");
    }

    window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 640, 480,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr) {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
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

    if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0) {
        throw std::logic_error("error: failed to initialize glad");
    }

    auto vert_shader     = comiple_vertex_shader(default_shaders::vertex);
    auto fragment_shader = comiple_fragment_shader(default_shaders::fragment);

    program_id_ = glCreateProgram();
    gl_error_check();
    if (0 == program_id_) {
        throw std::runtime_error("failed to create gl program");
    }

    glAttachShader(program_id_, vert_shader);
    gl_error_check();
    glAttachShader(program_id_, fragment_shader);
    gl_error_check();

    // bind attribute location
    glBindAttribLocation(program_id_, 0, "a_position");
    gl_error_check();
    // link program after binding attribute locations
    glLinkProgram(program_id_);
    gl_error_check();
    // Check the link status
    GLint linked_status = 0;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
    gl_error_check();

    if (linked_status == 0) {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        gl_error_check();
        serr << "Error linking program:\n" << infoLog.data();
        glDeleteProgram(program_id_);

        const int err = static_cast<int>(glGetError());
        if (err != GL_NO_ERROR)
            throw std::logic_error("OpenGL failed to load");
    }

    glUseProgram(program_id_);

    if (already_exist) {
        throw std::runtime_error("engine already exist");
    }
    already_exist = true;

    glValidateProgram(program_id_);
    gl_error_check();
}

sdl_engine::~sdl_engine() {
    if (already_exist == false) {
        throw std::runtime_error("engine not created");
    }
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
    GLintptr position_attr_offset = 0;

    gl_error_check();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex) / 2,
                          &t.v[0]);
    gl_error_check();
    glEnableVertexAttribArray(0);
    gl_error_check();

    glValidateProgram(program_id_);
    gl_error_check();

    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id_, GL_VALIDATE_STATUS, &validate_status);
    gl_error_check();
    if (validate_status == GL_FALSE) {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        gl_error_check();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        gl_error_check();
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    gl_error_check();
}

void sdl_engine::swap_buffers() {
    SDL_GL_SwapWindow(window);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    gl_error_check();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_error_check();
}

} // namespace eng
