#ifndef _RAY_H_
#define _RAY_H_

#include "constant.h"
#include "vec.h"

struct Ray {
    Vec org, dir;

    Ray() : org(), dir() {}
    Ray(const Vec &org, const Vec &dir) : org(org), dir(dir) {}
};

struct Hitpoint {
    double distance;
    Vec normal;
    Vec position;
    double u, v;

    Hitpoint() : distance(INF), normal(), position(), u(0.0), v(0.0) {}
};

struct Intersection {
    Hitpoint hitpoint;
    int object_id;

    Intersection() : hitpoint(), object_id(-1) {}
};

#endif
