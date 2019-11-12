#include "engine.hpp"

#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <SDL2/SDL.h>

namespace custom_engine {

static bool already_exist = false;

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

    SDL_Window* const window =
        SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 640, 480, ::SDL_WINDOW_OPENGL);

    if (window == nullptr) {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        throw std::runtime_error("Failed to create a window");
    }
    if (already_exist) {
        throw std::runtime_error("engine already exist");
    }
    already_exist = true;
}

sdl_engine::~sdl_engine() noexcept(false) {
    if (already_exist == false) {
        throw std::runtime_error("engine not created");
    }
    if (this == nullptr) {
        throw std::runtime_error("engine is a nullpointer");
    }
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

} // namespace custom_engine
