#pragma once

#include <iostream>

namespace matrix {

struct vector {
    float x = 0;
    float y = 0;
    float z = 0;
    constexpr vector() {
        x, y = 0;
        z = 0;
    }
    constexpr vector(float _x, float _y)
        : x{ _x }
        , y{ _y } {}
    constexpr vector(std::pair<float, float> p)
        : x{ p.first }
        , y{ p.second } {}
};

struct matrix {
    vector r1;
    vector r2;
    vector r3;
    matrix transpose();
};

constexpr inline matrix identity() {
    matrix m;

    m.r1.x = 1.f;
    m.r2.y = 1.f;
    m.r3.z = 1.f;

    return m;
}
constexpr matrix operator*(const matrix& lh, int rh);
constexpr matrix operator*(const matrix& lh, const matrix& rh) {
    matrix out;

    out.r1.x = (lh.r1.x * rh.r1.x) + (lh.r1.y * rh.r2.x) + (lh.r1.z * rh.r3.x);
    out.r1.y = (lh.r1.x * rh.r1.y) + (lh.r1.y * rh.r2.y) + (lh.r1.z * rh.r3.y);
    out.r1.z = (lh.r1.x * rh.r1.z) + (lh.r1.y * rh.r2.z) + (lh.r1.z * rh.r3.z);

    out.r2.x = (lh.r2.x * rh.r1.x) + (lh.r2.y * rh.r2.x) + (lh.r2.z * rh.r3.x);
    out.r2.y = (lh.r2.x * rh.r1.y) + (lh.r2.y * rh.r2.y) + (lh.r2.z * rh.r3.y);
    out.r2.z = (lh.r2.x * rh.r1.z) + (lh.r2.y * rh.r2.z) + (lh.r2.z * rh.r3.z);

    out.r3.x = (lh.r3.x * rh.r1.x) + (lh.r3.y * rh.r2.x) + (lh.r3.z * rh.r3.x);
    out.r3.y = (lh.r3.x * rh.r1.y) + (lh.r3.y * rh.r2.y) + (lh.r3.z * rh.r3.y);
    out.r3.z = (lh.r3.x * rh.r1.z) + (lh.r3.y * rh.r2.z) + (lh.r3.z * rh.r3.z);

    return out;
}
// this is defined as per vertical vector
constexpr vector operator*(const vector& lh, const matrix& rh) {
    vector out;

    // if we have matrix [a b c] and vector [x y z] (actually [x y 1])

    // a * x + b * y + c
    // we do not need to multiply c by vector->z since in a simple 2D game z
    // will always be equal to one
    out.x = (rh.r1.x * lh.x) + (rh.r1.y * lh.y) + (rh.r1.z);
    out.y = (rh.r2.x * lh.x) + (rh.r2.y * lh.y) + (rh.r2.z);
    out.z = 1; // constant value to be left unchanged

    return out;
}
vector        operator+(const vector& lh, const vector& rh);
std::istream& operator>>(std::istream& is, matrix& x);
std::istream& operator>>(std::istream& is, vector& m);

matrix           scale(float x_factor, float y_factor);
matrix           rotate(float theta);
matrix           counter_clockwise_90();
matrix           y_reflect();
constexpr matrix move(const vector& v) {
    const auto func = [&]() -> matrix {
        auto lambda_matrix = identity();
        lambda_matrix.r1.z = v.x;
        lambda_matrix.r2.z = v.y;
        return lambda_matrix;
    };
    return func();
}

} // namespace matrix