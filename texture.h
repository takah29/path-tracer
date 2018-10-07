#ifndef _TEXTURE_H_
#define _TEXTURE_H_

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
        const double v =
            sin(M_PI * x / size + EPS) * sin(M_PI * y / size + EPS) * sin(M_PI * z / size + EPS);

        if (v < 0.0) {
            return color1;
        } else {
            return color2;
        }
    }
};

#endif
