#ifndef _RAY_H_
#define _RAY_H_

#include "constant.hpp"
#include "vec.hpp"

struct Ray {
    Vec org, dir;

    Ray();
    Ray(const Vec &org, const Vec &dir);
};

struct Hitpoint {
    double distance;
    Vec normal;
    Vec position;
    double u, v;

    Hitpoint();
};

struct Intersection {
    Hitpoint hitpoint;
    int object_id;

    Intersection();;
};

#endif
