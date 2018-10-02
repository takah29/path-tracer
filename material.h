#ifndef _MATERIAL_H_
#define _MATERIAL_H_

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
    Color color;
    Color emission;
    ReflectionType reflection_type;

    Material()
        : color(Color(0.0, 0.0, 0.0)),
          emission(Color(0.0, 0.0, 0.0)),
          reflection_type(ReflectionType::DIFFUSE) {}
    Material(const Color &color, const Color &emission, const ReflectionType &reflection_type)
        : color(color), emission(emission), reflection_type(reflection_type) {}
};

#endif
