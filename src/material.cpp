#include "material.h"

Material::Material()
    : color_ptr(nullptr), emission(Vec(0.0, 0.0, 0.0)), reflection_type(ReflectionType::DIFFUSE) {}
Material::Material(Texture *color_ptr, const Color &emission, const ReflectionType &reflection_type)
    : color_ptr(color_ptr), emission(emission), reflection_type(reflection_type) {}

void Material::set_color(Texture *color_ptr) { this->color_ptr = color_ptr; }
