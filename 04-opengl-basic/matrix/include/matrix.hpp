#pragma once

#include <iostream>

namespace matrix {

struct vector {
    float x = 0;
    float y = 0;
    float z = 0;
    constexpr vector() { x, y = 0;z = 0; }
    constexpr vector(float _x, float _y): x{_x},y{_y}{}
};

struct matrix {
    vector r1;
    vector r2;
    vector r3;
    matrix transpose();
};

matrix        identity();
matrix        operator*(const matrix& lh, int rh);
matrix        operator*(const matrix& lh, const matrix& rh);
vector        operator*(const vector& lh, const matrix& rh);
vector        operator+(const vector& lh, const vector& rh);
std::istream& operator>>(std::istream& is, matrix& x);
std::istream& operator>>(std::istream& is, vector& m);

matrix scale(float x_factor, float y_factor);
matrix rotate(float theta);
matrix counter_clockwise_90();
matrix y_reflect();
matrix move(const vector& x);

} // namespace matrix