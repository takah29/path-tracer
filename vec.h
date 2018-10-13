#ifndef _VEC_H_
#define _VEC_H_

#include <algorithm>
#include <cmath>
#include "constant.h"

using Point2D = std::pair<double, double>;

struct Vec {
    double x, y, z;

    Vec(const double x = 0.0, const double y = 0.0, const double z = 0.0) : x(x), y(y), z(z) {}

    inline Vec operator+(const Vec &rhs) const { return Vec(x + rhs.x, y + rhs.y, z + rhs.z); }
    inline Vec operator+(const double &rhs) const { return Vec(x + rhs, y + rhs, z + rhs); }
    inline Vec operator-(const Vec &rhs) const { return Vec(x - rhs.x, y - rhs.y, z - rhs.z); }
    inline Vec operator-(const double &rhs) const { return Vec(x - rhs, y - rhs, z - rhs); }
    inline Vec operator*(const double rhs) const { return Vec(x * rhs, y * rhs, z * rhs); }
    inline Vec operator/(const double rhs) const { return Vec(x / rhs, y / rhs, z / rhs); }

    inline bool operator==(const Vec &rhs) const {
        return std::abs(x - rhs.x) < EPS && std::abs(y - rhs.y) < EPS && std::abs(z - rhs.z) < EPS;
    }
    inline bool operator!=(const Vec &rhs) const { return !(*this == rhs); }
    inline bool operator>=(const Vec &rhs) const {
        return std::abs(x - rhs.x) >= EPS && std::abs(y - rhs.y) >= EPS &&
               std::abs(z - rhs.z) >= EPS;
    }
    inline bool operator>(const Vec &rhs) const { return *this >= rhs && *this != rhs; }
    inline bool operator<=(const Vec &rhs) const {
        return std::abs(x - rhs.x) <= EPS && std::abs(y - rhs.y) <= EPS &&
               std::abs(z - rhs.z) <= EPS;
    }
    inline bool operator<(const Vec &rhs) const { return *this <= rhs && *this != rhs; }

    inline void operator+=(const Vec &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }
    inline void operator+=(const double &rhs) {
        x += rhs;
        y += rhs;
        z += rhs;
    }
    inline void operator-=(const Vec &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
    }
    inline void operator-=(const double &rhs) {
        x -= rhs;
        y -= rhs;
        z -= rhs;
    }
    inline void operator*=(const double &rhs) {
        x *= rhs;
        y *= rhs;
        z *= rhs;
    }
    inline void operator/=(const double &rhs) {
        x /= rhs;
        y /= rhs;
        z /= rhs;
    }

    inline void normalize() {
        double l = sqrt(x * x + y * y + z * z);
        x /= l;
        y /= l;
        z /= l;
    }
};

inline Vec operator*(double lhs, const Vec &rhs) { return rhs * lhs; }

inline double dot(const Vec &v1, const Vec &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

inline double norm(const Vec &v) { return sqrt(dot(v, v)); }

inline Vec normalize(const Vec &v) { return v / norm(v); }

inline Vec multiply(const Vec &v1, const Vec &v2) {
    return Vec(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline Vec cross(const Vec &v1, const Vec &v2) {
    return Vec((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z),
               (v1.x * v2.y) - (v1.y * v2.x));
}

inline Vec max(const Vec &v1, const Vec &v2) {
    return Vec(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}

inline Vec min(const Vec &v1, const Vec &v2) {
    return Vec(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

using Color = Vec;
using Point = Vec;

#endif
