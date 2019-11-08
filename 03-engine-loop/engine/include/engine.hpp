#include <iosfwd>
#include <string>
#include <string_view>

namespace custom_engine {
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

std::ostream& operator<<(std::ostream& stream, const event e);

class engine {
public:
    /// pool event from input queue
    /// return true if more events in queue
    virtual bool read_input(event& e) = 0;
};

engine* new_sdl_engine();
} // namespace custom_engine
