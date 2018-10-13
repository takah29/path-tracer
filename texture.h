#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include "lattice_noise.h"
#include "ray.h"
#include "vec.h"

struct Texture {
    virtual Color value(const Hitpoint &hitpoint) const = 0;
};

struct ConstantTexture : public Texture {
    Color color;

    ConstantTexture() : color(0.5, 0.5, 0.5) {}
    ConstantTexture(const Color &color) : color(color) {}

    Color value(const Hitpoint &hitpoint) const { return color; }
};

struct CheckerTexture : public Texture {
    Color color1, color2;
    double size;

    CheckerTexture() : color1(1.0, 1.0, 1.0), color2(0.0, 0.0, 0.0), size(1.0) {}
    CheckerTexture(const Color &color1, const Color &color2, const double size = 1.0)
        : color1(color1), color2(color2), size(size) {}

    Color value(const Hitpoint &hitpoint) const {
        auto[x, y, z] = hitpoint.position;
        const double v = std::sin(M_PI * x / size + EPS) * std::sin(M_PI * y / size + EPS) *
                         std::sin(M_PI * z / size + EPS);

        if (v < 0.0) {
            return color1;
        } else {
            return color2;
        }
    }
};

struct FBmTexture : public Texture {
    LatticeNoise *noise_ptr;
    Color color;
    double min_value, max_value;

    FBmTexture() : noise_ptr(nullptr), color(0.0, 0.0, 0.0), min_value(0.0), max_value(1.0) {}
    FBmTexture(LatticeNoise *noise_ptr, const Vec &color, const double &min_value,
               const double &max_value)
        : noise_ptr(noise_ptr), color(color), min_value(min_value), max_value(max_value) {}

    void set_noise_ptr(LatticeNoise *noise_ptr) { this->noise_ptr = noise_ptr; }

    Color value(const Hitpoint &hitpoint) const {
        double v = noise_ptr->value_fBm(hitpoint.position);
        v = min_value + (max_value - min_value) * v;

        return v * color;
    }
};

struct WrappedFBmTexture : public Texture {
    LatticeNoise *noise_ptr;
    Color color;
    double expansion_number;

    WrappedFBmTexture() : noise_ptr(nullptr), color(0.0, 0.0, 0.0), expansion_number(0.0) {}
    WrappedFBmTexture(LatticeNoise *noise_ptr, const Vec &color, const double &expansion_number)
        : noise_ptr(noise_ptr), color(color), expansion_number(expansion_number) {}

    void set_noise_ptr(LatticeNoise *noise_ptr) { this->noise_ptr = noise_ptr; }

    Color value(const Hitpoint &hitpoint) const {
        double noise = expansion_number * noise_ptr->value_fBm(hitpoint.position);
        double v = noise - floor(noise);

        return v * color;
    }
};

struct MarbleTexture : public Texture {
    LatticeNoise *noise_ptr;
    Color color;
    double size, range;

    MarbleTexture() : noise_ptr(nullptr), color(0.0, 0.0, 0.0), size(1.0), range(1.0) {}
    MarbleTexture(LatticeNoise *noise_ptr, const Vec &color, const double &size,
                  const double &range)
        : noise_ptr(noise_ptr), color(color), size(size), range(range) {}

    void set_noise_ptr(LatticeNoise *noise_ptr) { this->noise_ptr = noise_ptr; }

    Color value(const Hitpoint &hitpoint) const {
        double v = 1.0 + std::sin((hitpoint.position.x +
                                   range * noise_ptr->value_turbulence(hitpoint.position)) /
                                  size);

        return v * color;
    }
};

#endif
