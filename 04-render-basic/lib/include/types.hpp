#pragma once

#include <algorithm>
#include <exception>
#include <stdint.h>

namespace draw_utils {

struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    constexpr color(uint8_t _r, uint8_t _g, uint8_t _b)
        : r{ _r }
        , g{ _g }
        , b{ _b } {
        if (r != std::clamp(static_cast<int>(r), 0, 255) ||
            g != std::clamp(static_cast<int>(g), 0, 255) ||
            b != std::clamp(static_cast<int>(b), 0, 255))
            throw std::logic_error("Invalid RGB values");
    }
    constexpr color()
        : r{ 0 }
        , g{ 0 }
        , b{ 0 } {
        if (r != std::clamp(static_cast<int>(r), 0, 255) ||
            g != std::clamp(static_cast<int>(g), 0, 255) ||
            b != std::clamp(static_cast<int>(b), 0, 255))
            throw std::logic_error("Invalid RGB values");
    }
};

struct pixel {
    int32_t x = 0;
    int32_t y = 0;
};

template <typename T>
inline T delta(T w1, T w2) {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return std::abs(w1 - w2);
}

} // namespace draw_utils