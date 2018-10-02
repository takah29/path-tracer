#ifndef _RAY_H_
#define _RAY_H_

#include "constant.h"
#include "vec.h"

struct Ray {
    Vec org, dir;

    Ray() {}
    Ray(const Vec &org, const Vec &dir) : org(org), dir(dir) {}
};

struct Hitpoint {
    double distance;
    Vec normal;
    Vec position;

    Hitpoint() : distance(INF), normal(), position() {}
};

struct Intersection {
    Hitpoint hitpoint;
    int object_id;

    Intersection() : hitpoint(Hitpoint()), object_id(-1) {}
};

#endif
