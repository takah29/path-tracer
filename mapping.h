#ifndef _MAPPING_H_
#define _MAPPING_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>
#include "ray.h"
#include "utility.h"
#include "vec.h"

struct Mapping {
    virtual ~Mapping() {}
    virtual Point2D get_uv_values(const Vec &v) const = 0;

    inline std::pair<int, int> get_texel_coordinates(const Hitpoint &hitpoint, const int width_res,
                                                     const int height_res) {
        auto[val0, val1] = get_uv_values(hitpoint.normal);
        val0 = val0 * (width_res - 1);
        val1 = (1.0 - val1) * (height_res - 1);
        return {static_cast<int>(val1), static_cast<int>(val0)};
    }
};

struct SphericalMap : public Mapping {
    double rotate_rad;

    SphericalMap(const int degree = 0) : rotate_rad(2 * M_PI * (degree % 360) / 360.0) {}
    ~SphericalMap() {}

    Point2D get_uv_values(const Vec &normalized_hitpoint) const {
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
};

#endif