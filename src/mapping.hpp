#ifndef _MAPPING_H_
#define _MAPPING_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>
#include "ray.hpp"
#include "utility.hpp"
#include "vec.hpp"

struct Mapping {
    virtual Point2D get_uv_values(const Vec &v) const = 0;
    virtual ~Mapping() {}
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

    SphericalMap();
    SphericalMap(const int degree);
    ~SphericalMap() {}

    Point2D get_uv_values(const Vec &normalized_hitpoint) const;
};

#endif
