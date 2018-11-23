#include "object.hpp"
#include <cassert>

BBox::BBox()
    : corner0(-0.5, -0.5, -0.5), corner1(0.5, 0.5, 0.5), center((corner0 + corner1) / 2.0) {}
BBox::BBox(const Vec &corner0, const Vec &corner1)
    : corner0(corner0), corner1(corner1), center((corner0 + corner1) / 2.0) {
    if (this->corner0.x > this->corner1.x) std::swap(this->corner0.x, this->corner1.x);
    if (this->corner0.y > this->corner1.y) std::swap(this->corner0.y, this->corner1.y);
    if (this->corner0.z > this->corner1.z) std::swap(this->corner0.z, this->corner1.z);
}

BBox::~BBox() {}

void BBox::set_corner(const Vec &corner0, const Vec &corner1) {
    this->corner0 = corner0;
    this->corner1 = corner1;
    if (this->corner0.x > this->corner1.x) std::swap(this->corner0.x, this->corner1.x);
    if (this->corner0.y > this->corner1.y) std::swap(this->corner0.y, this->corner1.y);
    if (this->corner0.z > this->corner1.z) std::swap(this->corner0.z, this->corner1.z);
    this->center = (this->corner0 + this->corner1) / 2.0;
}

void BBox::empty() {
    this->corner0 = Vec(INF, INF, INF);
    this->corner1 = Vec(-INF, -INF, -INF);
    this->center = Vec(0.0, 0.0, 0.0);
}

bool BBox::hit(const Ray &ray) const {
    const double ox = ray.org.x, oy = ray.org.y, oz = ray.org.z;
    const double dx = ray.dir.x, dy = ray.dir.y, dz = ray.dir.z;

    double tx_min, ty_min, tz_min;
    double tx_max, ty_max, tz_max;

    const double a = 1.0 / dx;
    if (a >= 0.0) {
        tx_min = (corner0.x - ox) * a;
        tx_max = (corner1.x - ox) * a;
    } else {
        tx_min = (corner1.x - ox) * a;
        tx_max = (corner0.x - ox) * a;
    }

    const double b = 1.0 / dy;
    if (b >= 0.0) {
        ty_min = (corner0.y - oy) * b;
        ty_max = (corner1.y - oy) * b;
    } else {
        ty_min = (corner1.y - oy) * b;
        ty_max = (corner0.y - oy) * b;
    }

    const double c = 1.0 / dz;
    if (c >= 0.0) {
        tz_min = (corner0.z - oz) * c;
        tz_max = (corner1.z - oz) * c;
    } else {
        tz_min = (corner1.z - oz) * c;
        tz_max = (corner0.z - oz) * c;
    }

    double t0 = std::max({tx_min, ty_min, tz_min});
    double t1 = std::min({tx_max, ty_max, tz_max});

    return ((t0 < t1) && (t1 > EPS));
}

Object::Object()
    : center(0.0, 0.0, 0.0),
      bbox(Vec(INF, INF, INF), Vec(-INF, -INF, -INF)),
      material_ptr(nullptr) {}
Object::Object(const Vec &center, const BBox &bbox, Material *material_ptr)
    : center(center), bbox(bbox), material_ptr(material_ptr) {}
Object::~Object() {}

void Object::set_material(Material *material_ptr) { this->material_ptr = material_ptr; }

Sphere::Sphere() : Object(), radius(1.0) {
    bbox.corner0 = Vec(-1.0, -1.0, -1.0) - EPS;
    bbox.corner1 = Vec(1.0, 1.0, 1.0) + EPS;
}
Sphere::Sphere(const double radius, const Vec &center, Material *material_ptr)
    : Object(center, BBox(center - (radius + EPS), center + (radius + EPS)), material_ptr),
      radius(radius) {}
Sphere::~Sphere() {}

bool Sphere::intersect(const Ray &ray, Hitpoint &hitpoint) const {
    const Vec p_o = center - ray.org;
    const double b = dot(p_o, ray.dir);
    const double D4 = b * b - dot(p_o, p_o) + radius * radius;

    if (D4 < 0.0) return false;

    const double sqrt_D4 = sqrt(D4);
    const double t1 = b - sqrt_D4, t2 = b + sqrt_D4;

    if (t2 <= EPS) {
        return false;
    } else if (t1 > EPS) {
        hitpoint.distance = t1;
    } else {
        hitpoint.distance = t2;
    }

    hitpoint.position = ray.org + hitpoint.distance * ray.dir;
    hitpoint.normal = normalize(hitpoint.position - center);

    return true;
}

Plane::Plane() : Object(), normal(0.0, 1.0, 0.0) {
    bbox.corner0 = Vec(-INF, -EPS, -INF);
    bbox.corner1 = Vec(INF, EPS, INF);
}
Plane::Plane(const Vec &center, const Vec &normal, Material *material_ptr)
    : Object(center, BBox(Vec(-INF, -EPS, -INF) + center, Vec(INF, EPS, INF) + center),
             material_ptr),
      normal(normalize(normal)) {}
Plane::~Plane() {}

bool Plane::intersect(const Ray &ray, Hitpoint &hitpoint) const {
    const double t = dot((center - ray.org), normal) / dot(ray.dir, normal);

    if (t > EPS) {
        hitpoint.distance = t;
        hitpoint.normal = normal;
        hitpoint.position = ray.org + t * ray.dir;

        return true;
    }

    return false;
}

Box::Box() : Object(), corner0(-0.5, -0.5, -0.5), corner1(0.5, 0.5, 0.5) {
    bbox.corner0 = corner0 - EPS;
    bbox.corner1 = corner1 + EPS;
}

Box::Box(const Vec &corner0, const Vec &corner1, Material *material_ptr)
    : Object((corner0 + corner1) / 2, BBox(corner0 - EPS, corner1 + EPS), material_ptr),
      corner0(corner0),
      corner1(corner1) {
    if (this->corner0.x > this->corner1.x) std::swap(this->corner0.x, this->corner1.x);
    if (this->corner0.y > this->corner1.y) std::swap(this->corner0.y, this->corner1.y);
    if (this->corner0.z > this->corner1.z) std::swap(this->corner0.z, this->corner1.z);
}

Box::~Box() {}

Vec Box::get_normal(const int face_hit) const {
    switch (face_hit) {
        case 0:
            return Vec(-1.0, 0.0, 0.0);
        case 1:
            return Vec(0.0, -1.0, 0.0);
        case 2:
            return Vec(0.0, 0.0, -1.0);
        case 3:
            return Vec(1.0, 0.0, 0.0);
        case 4:
            return Vec(0.0, 1.0, 0.0);
        case 5:
            return Vec(0.0, 0.0, 1.0);
        default:
            assert(!"Error: 0 <= face_hit <= 5 expected.");
            return Vec();
    }
}

bool Box::intersect(const Ray &ray, Hitpoint &hitpoint) const {
    const double ox = ray.org.x, oy = ray.org.y, oz = ray.org.z;
    const double dx = ray.dir.x, dy = ray.dir.y, dz = ray.dir.z;

    double tx_min, ty_min, tz_min;
    double tx_max, ty_max, tz_max;

    const double a = 1.0 / dx;
    const bool a_ge = (a >= 0.0);
    if (a_ge) {
        tx_min = (corner0.x - ox) * a;
        tx_max = (corner1.x - ox) * a;
    } else {
        tx_min = (corner1.x - ox) * a;
        tx_max = (corner0.x - ox) * a;
    }

    const double b = 1.0 / dy;
    const bool b_ge = (b >= 0.0);
    if (b_ge) {
        ty_min = (corner0.y - oy) * b;
        ty_max = (corner1.y - oy) * b;
    } else {
        ty_min = (corner1.y - oy) * b;
        ty_max = (corner0.y - oy) * b;
    }

    const double c = 1.0 / dz;
    const bool c_ge = (c >= 0.0);
    if (c_ge) {
        tz_min = (corner0.z - oz) * c;
        tz_max = (corner1.z - oz) * c;
    } else {
        tz_min = (corner1.z - oz) * c;
        tz_max = (corner0.z - oz) * c;
    }

    double t0, t1;
    int face_in, face_out;

    if (tx_min > ty_min) {
        t0 = tx_min;
        face_in = a_ge ? 0 : 3;
    } else {
        t0 = ty_min;
        face_in = b_ge ? 1 : 4;
    }

    if (tz_min > t0) {
        t0 = tz_min;
        face_in = c_ge ? 2 : 5;
    }

    if (tx_max < ty_max) {
        t1 = tx_max;
        face_out = a_ge ? 3 : 0;
    } else {
        t1 = ty_max;
        face_out = b_ge ? 4 : 1;
    }

    if (tz_max < t1) {
        t1 = tz_max;
        face_out = c_ge ? 5 : 2;
    }

    double t;
    if (t0 < t1 && t1 > EPS) {
        if (t0 > EPS) {
            t = t0;
            hitpoint.normal = get_normal(face_in);
        } else {
            t = t1;
            hitpoint.normal = get_normal(face_out);
        }
        hitpoint.position = ray.org + t * ray.dir;
        hitpoint.distance = t;
        return true;
    } else {
        return false;
    }
}

Triangle::Triangle()
    : Object(), v0(0.0, 0.0, 0.0), v1(0.0, 0.0, 1.0), v2(1.0, 0.0, 0.0), normal(0.0, 1.0, 0.0) {
    bbox.corner0 = min(min(v0, v1), v2) - EPS;
    bbox.corner1 = max(max(v0, v1), v2) + EPS;
}

Triangle::Triangle(const Vec &v0, const Vec &v1, const Vec &v2, Material *material_ptr)
    : Object((v0 + v1 + v2) / 3.0, BBox(min(min(v0, v1), v2) - EPS, max(max(v0, v1), v2) + EPS),
             material_ptr),
      v0(v0),
      v1(v1),
      v2(v2) {
    normal = cross(v1 - v0, v2 - v0);
    normal.normalize();
}

Triangle::~Triangle() {}

bool Triangle::intersect(const Ray &ray, Hitpoint &hitpoint) const {
    auto [a, e, i] = v0 - v1;
    auto [b, f, j] = v0 - v2;
    auto [c, g, k] = ray.dir;
    auto [d, h, l] = v0 - ray.org;

    double m = f * k - g * j, n = h * k - g * l, p = f * l - h * j;
    double q = g * i - e * k, s = e * j - f * i;

    double D = (a * m + b * q + c * s);

    double e1 = d * m - b * n - c * p;
    double beta = e1 / D;

    if (beta < 0.0) {
        return false;
    }

    double r = e * l - h * i;
    double e2 = a * n + d * q + c * r;
    double gamma = e2 / D;

    if (gamma < 0.0) {
        return false;
    }

    if (beta + gamma > 1.0) {
        return false;
    }

    double e3 = a * p - b * r + d * s;
    double t = e3 / D;

    if (t < EPS) {
        return false;
    }

    hitpoint.distance = t;
    hitpoint.position = ray.org + t * ray.dir;
    hitpoint.normal = normal;

    return true;
}
