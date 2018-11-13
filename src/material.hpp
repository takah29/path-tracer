#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "texture.hpp"
#include "vec.hpp"

const Color RED = Color(1.0, 0.0, 0.0);
const Color GREEN = Color(0.0, 1.0, 0.0);
const Color BLUE = Color(0.0, 0.0, 1.0);
const Color WHITE = Color(1.0, 1.0, 1.0);
const Color CYAN = Color(0.0, 1.0, 1.0);
const Color MAGENTA = Color(1.0, 0.0, 1.0);
const Color YELLOW = Color(1.0, 1.0, 0.0);
const Color BLACK = Color(0.0, 0.0, 0.0);
const Color GRAY = Color(0.75, 0.75, 0.75);

enum ReflectionType {
    DIFFUSE,
    SPECULAR,
    REFRACTION,
};

struct Material {
    Texture *color_ptr;
    Color emission;
    ReflectionType reflection_type;
    bool texture_flag;

    Material();
    Material(Texture *color_ptr, const Color &emission, const ReflectionType &reflection_type);

    void set_color(Texture *color_ptr);
};

#endif
