#include "camera.hpp"

Camera::Camera(Vec eye, Vec lookat) : eye(eye), lookat(lookat), up(0.0, 1.0, 0.0) {
    w = normalize(eye - lookat);
    u = normalize(cross(up, w));
    v = cross(w, u);
}
Camera::~Camera() {}

Pinhole::Pinhole(Vec eye, Vec lookat, float dist) : Camera(eye, lookat), dist(dist) {}
Pinhole::~Pinhole() {}

Vec Pinhole::ray_direction(const Point2D& p) const {
    Vec dir = p.first * u + p.second * v - dist * w;
    dir.normalize();
    return dir;
}
