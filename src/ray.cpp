#include "ray.hpp"

Ray::Ray() : org(), dir() {}
Ray::Ray(const Vec &org, const Vec &dir) : org(org), dir(dir) {}

Hitpoint::Hitpoint() : distance(INF), normal(), position(), u(0.0), v(0.0) {}

Intersection::Intersection() : hitpoint(), object_id(-1) {}
