#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "texture.h"
#include "vec.h"

const Color RED = Vec(1.0, 0.0, 0.0);
const Color GREEN = Vec(0.0, 1.0, 0.0);
const Color BLUE = Vec(0.0, 0.0, 1.0);
const Color WHITE = Vec(1.0, 1.0, 1.0);
const Color CYAN = Vec(0.0, 1.0, 1.0);
const Color MAGENTA = Vec(1.0, 0.0, 1.0);
const Color YELLOW = Vec(1.0, 1.0, 0.0);
const Color BLACK = Vec(0.0, 0.0, 0.0);

enum ReflectionType {
    DIFFUSE,
    SPECULAR,
    REFRACTION,
};

struct Material {
    Texture *color_ptr;
    Color emission;
    ReflectionType reflection_type;

    Material()
        : color_ptr(nullptr), emission(Vec(0.0, 0.0, 0.0)), reflection_type(ReflectionType::DIFFUSE) {}
    Material(Texture *color_ptr, const Color &emission, const ReflectionType &reflection_type)
        : color_ptr(color_ptr), emission(emission), reflection_type(reflection_type) {}

    void set_color(Texture *color_ptr) { this->color_ptr = color_ptr; }
};

#endif
