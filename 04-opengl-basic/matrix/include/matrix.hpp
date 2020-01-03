#pragma once

#include <iostream>

namespace matrix {

struct vector {
    float x = 0;
    float y = 0;
    float z = 0;
    vector() { x, y, z = 0; }
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

matrix scale(float x_factor,float y_factor);
matrix rotate(float theta);
matrix y_reflect();
matrix move(const vector& x);

} // namespace matrix