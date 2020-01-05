#include "matrix.hpp"
#include <algorithm>
#include <cmath>

namespace matrix {

matrix matrix::transpose() {
    std::swap(r1.y, r2.x);
    std::swap(r1.z, r3.x);
    std::swap(r2.z, r3.y);
    return *this;
}

constexpr matrix operator*(const matrix& lh, int rh) {
    matrix out;

    out.r1.x = lh.r1.x * rh;
    out.r1.y = lh.r1.y * rh;
    out.r1.z = lh.r1.z * rh;
    out.r2.x = lh.r2.x * rh;
    out.r2.y = lh.r2.y * rh;
    out.r2.z = lh.r2.z * rh;
    out.r3.x = lh.r3.x * rh;
    out.r3.y = lh.r3.y * rh;
    out.r3.z = lh.r3.z * rh;

    return out;
}

vector operator+(const vector& lh, const vector& rh) {
    vector out;

    out.x = lh.x + rh.x;
    out.y = lh.y + rh.y;
    out.z = lh.z + rh.z;

    return out;
}

std::istream& operator>>(std::istream& is, matrix& m) {
    is >> m.r1.x >> m.r1.y >> m.r1.z;
    is >> m.r2.x >> m.r2.y >> m.r2.z;
    is >> m.r3.x >> m.r3.y >> m.r3.z;

    return is;
}

std::istream& operator>>(std::istream& is, vector& m) {
    is >> m.x >> m.y;
    m.z = 1;
    return is;
}

matrix scale(float x_factor, float y_factor) {
    auto scale = identity();

    scale.r1.x = x_factor;
    scale.r2.y = y_factor;

    return scale;
}

matrix rotate(float theta) {
    auto out = identity();
    out.r1.x = std::cos(theta);
    out.r1.y = std::sin(theta);
    out.r2.x = -std::sin(theta);
    out.r2.y = std::cos(theta);

    return out;
}
matrix y_reflect() {
    auto scale = identity();

    scale.r1.x = -1;

    return scale;
}

matrix counter_clockwise_90() {
    auto out = identity();
    out.r1.x = 0.f;
    out.r1.y = -1.f;
    out.r2.x = 1.f;
    out.r2.y = 0.f;

    return out;
}

} // namespace matrix