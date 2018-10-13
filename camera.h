#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "utility.h"
#include "vec.h"

struct Camera {
    Vec eye, lookat;
    Vec up;
    Vec u, v, w;
    float exposure_time;

    Camera(Vec eye, Vec lookat) : eye(eye), lookat(lookat), up(0.0, 1.0, 0.0) {
        w = normalize(eye - lookat);
        u = normalize(cross(up, w));
        v = cross(w, u);
    }

    virtual ~Camera() {}

    virtual Vec ray_direction(const Point2D& p) const = 0;
};

struct Pinhole : public Camera {
    float dist;

    Pinhole(Vec eye, Vec lookat, float dist) : Camera(eye, lookat), dist(dist) {}

    ~Pinhole() {}

    Vec ray_direction(const Point2D& p) const {
        Vec dir = p.first * u + p.second * v - dist * w;
        dir.normalize();
        return dir;
    }
};

#endif
