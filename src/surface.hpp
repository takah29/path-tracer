#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "bvh.hpp"
#include "material.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "utility.hpp"

struct Surface : public Object {
    std::vector<Vec> vertices;
    std::vector<std::tuple<int, int, int>> triangles;
    std::vector<BBox> triangle_bboxes;

    BVH bvh;

    Surface();
    Surface(Material *material_ptr);
    ~Surface();

    virtual void get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                            const double &gamma) const = 0;

    virtual void compute_normals() = 0;

    void scale(const double x);
    void move(const Vec x);
    void set_material(Material *material_ptr);
    void compute_bboxes();
    void construct();
    std::vector<int> traverse(const Ray &ray);

    bool intersect_triangle(const int &triangle_idx, const Ray &ray, Hitpoint &hitpoint) const;
    bool intersect(const Ray &ray, Hitpoint &hitpoint) const;
};

struct FlatSurface : public Surface {
    std::vector<Vec> triangle_normals;

    FlatSurface();
    FlatSurface(Material *material_ptr);
    ~FlatSurface();

    void compute_normals();
    void get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                    const double &gamma) const;
};

struct SmoothSurface : public Surface {
    std::vector<Vec> vertex_normals;

    SmoothSurface();
    SmoothSurface(Material *material_ptr);
    ~SmoothSurface();

    void compute_normals();
    void get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                    const double &gamma) const;
};

#endif
