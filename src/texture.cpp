#include "texture.h"
#include <cmath>

ConstantTexture::ConstantTexture() : color(0.5, 0.5, 0.5) {}
ConstantTexture::ConstantTexture(const Color &color) : color(color) {}
Color ConstantTexture::get_value(const Hitpoint &hitpoint) const { return color; }

ImageTexture::ImageTexture() : image_ptr(nullptr), mapping_ptr(nullptr) {}
ImageTexture::ImageTexture(Image *image_ptr, Mapping *mapping_ptr)
    : image_ptr(image_ptr), mapping_ptr(mapping_ptr) {}
ImageTexture::~ImageTexture() {
    delete image_ptr;
    delete mapping_ptr;
}
void ImageTexture::set_image(Image *image_ptr) { this->image_ptr = image_ptr; }
void ImageTexture::set_mapping(Mapping *mapping_ptr) { this->mapping_ptr = mapping_ptr; }
Color ImageTexture::get_value(const Hitpoint &hitpoint) const {
    int row, column;
    if (mapping_ptr) {
        std::tie(row, column) = mapping_ptr->get_texel_coordinates(hitpoint, image_ptr->width_res,
                                                                   image_ptr->height_res);
    } else {
        row = static_cast<int>((1.0 - hitpoint.v) * (image_ptr->height_res - 1));
        column = static_cast<int>(hitpoint.u * (image_ptr->width_res - 1));
    }

    return image_ptr->get_color(row, column);
}

CheckerTexture::CheckerTexture() : color1(1.0, 1.0, 1.0), color2(0.0, 0.0, 0.0), size(1.0) {}
CheckerTexture::CheckerTexture(const Color &color1, const Color &color2, const double size)
    : color1(color1), color2(color2), size(size) {}
Color CheckerTexture::get_value(const Hitpoint &hitpoint) const {
    auto[x, y, z] = hitpoint.position;
    const double v = std::sin(M_PI * x / size + EPS) * std::sin(M_PI * y / size + EPS) *
                     std::sin(M_PI * z / size + EPS);

    if (v < 0.0) {
        return color1;
    } else {
        return color2;
    }
}

FBmTexture::FBmTexture()
    : noise_ptr(nullptr), color(0.0, 0.0, 0.0), min_value(0.0), max_value(1.0) {}
FBmTexture::FBmTexture(LatticeNoise *noise_ptr, const Vec &color, const double &min_value,
                       const double &max_value)
    : noise_ptr(noise_ptr), color(color), min_value(min_value), max_value(max_value) {}
FBmTexture::~FBmTexture() { delete noise_ptr; }
void FBmTexture::set_noise_ptr(LatticeNoise *noise_ptr) { this->noise_ptr = noise_ptr; }
Color FBmTexture::get_value(const Hitpoint &hitpoint) const {
    double v = noise_ptr->value_fBm(hitpoint.position);
    v = min_value + (max_value - min_value) * v;

    return v * color;
}

WrappedFBmTexture::WrappedFBmTexture()
    : noise_ptr(nullptr), color(0.0, 0.0, 0.0), expansion_number(0.0) {}
WrappedFBmTexture::WrappedFBmTexture(LatticeNoise *noise_ptr, const Vec &color,
                                     const double &expansion_number)
    : noise_ptr(noise_ptr), color(color), expansion_number(expansion_number) {}
WrappedFBmTexture::~WrappedFBmTexture() { delete noise_ptr; }
void WrappedFBmTexture::set_noise_ptr(LatticeNoise *noise_ptr) { this->noise_ptr = noise_ptr; }
Color WrappedFBmTexture::get_value(const Hitpoint &hitpoint) const {
    double noise = expansion_number * noise_ptr->value_fBm(hitpoint.position);
    double v = noise - floor(noise);

    return v * color;
}

MarbleTexture::MarbleTexture() : noise_ptr(nullptr), color(0.0, 0.0, 0.0), size(1.0), range(1.0) {}
MarbleTexture::MarbleTexture(LatticeNoise *noise_ptr, const Vec &color, const double &size,
                             const double &range)
    : noise_ptr(noise_ptr), color(color), size(size), range(range) {}
MarbleTexture::~MarbleTexture() { delete noise_ptr; }
void MarbleTexture::set_noise_ptr(LatticeNoise *noise_ptr) { this->noise_ptr = noise_ptr; }
Color MarbleTexture::get_value(const Hitpoint &hitpoint) const {
    double v =
        1.0 +
        std::sin((hitpoint.position.x + range * noise_ptr->value_turbulence(hitpoint.position)) /
                 size);

    return v * color;
}
