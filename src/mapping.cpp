#include "mapping.hpp"

SphericalMap::SphericalMap() : rotate_rad(0.0) {}
SphericalMap::SphericalMap(const int degree) : rotate_rad(2 * M_PI * (degree % 360) / 360.0) {}

Point2D SphericalMap::get_uv_values(const Vec &normalized_hitpoint) const {
    auto[x, y, z] = normalized_hitpoint;

    double theta = std::acos(y);
    double phi = std::atan2(x, z) + rotate_rad;

    if (phi < 0.0) {
        phi += 2 * M_PI;
    } else if (phi > 2 * M_PI) {
        phi -= 2 * M_PI;
    }

    double u = phi / (2 * M_PI);
    double v = 1 - theta / M_PI;

    return {u, v};
}
