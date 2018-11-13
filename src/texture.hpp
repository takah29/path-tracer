#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define _USE_MATH_DEFINES

#include <tuple>
#include "lattice_noise.hpp"
#include "mapping.hpp"
#include "ray.hpp"
#include "utility.hpp"
#include "vec.hpp"

struct Texture {
    virtual ~Texture() {}
    virtual Color get_value(const Hitpoint &hitpoint) const = 0;
};

struct ConstantTexture : public Texture {
    Color color;

    ConstantTexture();
    ConstantTexture(const Color &color);

    Color get_value(const Hitpoint &hitpoint) const;
};

struct ImageTexture : public Texture {
    Image *image_ptr;
    Mapping *mapping_ptr;

    ImageTexture();
    ImageTexture(Image *image_ptr, Mapping *mapping_ptr);
    ~ImageTexture();

    void set_image(Image *image_ptr);
    void set_mapping(Mapping *mapping_ptr);
    Color get_value(const Hitpoint &hitpoint) const;
};

struct CheckerTexture : public Texture {
    Color color1, color2;
    double size;

    CheckerTexture();
    CheckerTexture(const Color &color1, const Color &color2, const double size = 1.0);

    Color get_value(const Hitpoint &hitpoint) const;
};

struct FBmTexture : public Texture {
    LatticeNoise *noise_ptr;
    Color color;
    double min_value, max_value;

    FBmTexture();
    FBmTexture(LatticeNoise *noise_ptr, const Vec &color, const double &min_value,
               const double &max_value);
    ~FBmTexture();

    void set_noise_ptr(LatticeNoise *noise_ptr);
    Color get_value(const Hitpoint &hitpoint) const;
};

struct WrappedFBmTexture : public Texture {
    LatticeNoise *noise_ptr;
    Color color;
    double expansion_number;

    WrappedFBmTexture();
    WrappedFBmTexture(LatticeNoise *noise_ptr, const Vec &color, const double &expansion_number);
    ~WrappedFBmTexture();

    void set_noise_ptr(LatticeNoise *noise_ptr);
    Color get_value(const Hitpoint &hitpoint) const;
};

struct MarbleTexture : public Texture {
    LatticeNoise *noise_ptr;
    Color color;
    double size, range;

    MarbleTexture();
    MarbleTexture(LatticeNoise *noise_ptr, const Vec &color, const double &size,
                  const double &range);
    ~MarbleTexture();

    void set_noise_ptr(LatticeNoise *noise_ptr);
    Color get_value(const Hitpoint &hitpoint) const;
};

#endif
