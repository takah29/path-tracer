#include "vec.hpp"
#include <iostream>

Vec::Vec(const double x, const double y, const double z) : x(x), y(y), z(z) {}

void Vec::normalize() {
    double l = sqrt(x * x + y * y + z * z);
    x /= l;
    y /= l;
    z /= l;
}

Vec spherical_coordinate_vec(const double theta, const double phi) {
    double x = std::sin(theta) * std::sin(phi);
    double y = std::cos(theta);
    double z = std::sin(theta) * std::cos(phi);
    if (x == -0.0) x = 0.0;
    if (z == -0.0) z = 0.0;
    return Vec(x, y, z);
}

std::ostream &operator<<(std::ostream &os, const Vec &v) {
    os << "Vec(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
