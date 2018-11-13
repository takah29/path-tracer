#include "material.hpp"

Material::Material()
    : color_ptr(new ConstantTexture(GRAY)),
      emission(Color(0.0, 0.0, 0.0)),
      reflection_type(ReflectionType::DIFFUSE),
      texture_flag(false) {}
Material::Material(Texture *color_ptr, const Color &emission, const ReflectionType &reflection_type)
    : color_ptr(color_ptr),
      emission(emission),
      reflection_type(reflection_type),
      texture_flag(false) {}

void Material::set_color(Texture *color_ptr) { this->color_ptr = color_ptr; }
