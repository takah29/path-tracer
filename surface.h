#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "bvh.h"
#include "material.h"
#include "object.h"
#include "ray.h"
#include "utility.h"

struct Surface : public Object {
    std::vector<Vec> vertices;
    std::vector<std::tuple<int, int, int>> triangles;
    std::vector<BBox> triangle_bboxes;
    BVH bvh;

    Surface() : Object(), vertices(0), triangles(0), triangle_bboxes(0) {}
    Surface(const Material &material)
        : Object(Vec(0.0, 0.0, 0.0), BBox(), material),
          vertices(0),
          triangles(0),
          triangle_bboxes(0) {
        this->bbox.empty();
    }

    ~Surface() {}

    virtual void get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                            const double &gamma) const = 0;

    virtual void compute_normals() = 0;

    void scale(const double x) {
        for (auto &v : vertices) {
            v *= x;
        }
        bbox.corner0 *= x;
        bbox.corner1 *= x;
        for (auto &bvh_node : bvh.bvh_nodes) {
            bvh_node.bbox.corner0 *= x;
            bvh_node.bbox.corner1 *= x;
        }
    }

    void move(const Vec x) {
        for (auto &v : vertices) {
            v += x;
        }
        bbox.corner0 += x;
        bbox.corner1 += x;
        for (auto &bvh_node : bvh.bvh_nodes) {
            bvh_node.bbox.corner0 += x;
            bvh_node.bbox.corner1 += x;
        }
    }

    void compute_bboxes() {
        for (size_t i = 0; i < triangles.size(); i++) {
            const auto & [ idx0, idx1, idx2 ] = triangles[i];
            const Vec &v0(vertices[idx0]), &v1(vertices[idx1]), &v2(vertices[idx2]);
            BBox bbox(min(min(v0, v1), v2) - EPS, max(max(v0, v1), v2) + EPS);
            triangle_bboxes.push_back(bbox);
        }
    }

    void construct() {
        std::vector<BBox *> bboxes(triangle_bboxes.size());
        for (size_t i = 0; i < triangle_bboxes.size(); i++) {
            bboxes[i] = &triangle_bboxes[i];
        }
        bvh.construct(bboxes);
    }

    std::vector<int> traverse(const Ray &ray) { return bvh.traverse(ray); }

    bool intersect_triangle(const int &triangle_idx, const Ray &ray, Hitpoint &hitpoint) const {
        const auto & [ idx0, idx1, idx2 ] = triangles[triangle_idx];
        const Vec &v0(vertices[idx0]), &v1(vertices[idx1]), &v2(vertices[idx2]);

        auto[a, e, i] = v0 - v1;
        auto[b, f, j] = v0 - v2;
        auto[c, g, k] = ray.dir;
        auto[d, h, l] = v0 - ray.org;

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
        get_normal(triangle_idx, hitpoint, beta, gamma);

        return true;
    }

    bool intersect(const Ray &ray, Hitpoint &hitpoint) const {
        bool result = false;
        for (int i : bvh.traverse(ray)) {
            Hitpoint tmp_hitpoint;
            if (intersect_triangle(i, ray, tmp_hitpoint)) {
                if (tmp_hitpoint.distance < hitpoint.distance) {
                    hitpoint = tmp_hitpoint;
                }
                result = true;
            }
        }

        return result;
    }
};

struct FlatSurface : public Surface {
    std::vector<Vec> triangle_normals;

    FlatSurface() : Surface(), triangle_normals(0) {}
    FlatSurface(const Material &material) : Surface(material), triangle_normals(0) {
        this->bbox.empty();
    }

    ~FlatSurface() {}

    void compute_normals() {
        Vec v0, v1, v2;
        Vec normal;
        triangle_normals = std::vector<Vec>(triangles.size());

        for (size_t i = 0; i < triangles.size(); i++) {
            Vec &v0 = vertices[std::get<0>(triangles[i])];
            Vec &v1 = vertices[std::get<1>(triangles[i])];
            Vec &v2 = vertices[std::get<2>(triangles[i])];

            normal = cross(v1 - v0, v2 - v0);
            normal.normalize();

            triangle_normals[i] = normal;
        }
    }

    void get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                    const double &gamma) const {
        hitpoint.normal = triangle_normals[triangle_idx];
    }
};

struct SmoothSurface : public Surface {
    std::vector<Vec> vertex_normals;

    SmoothSurface() : Surface(), vertex_normals(0) {}
    SmoothSurface(const Material &material) : Surface(material), vertex_normals(0) {
        this->bbox.empty();
    }

    ~SmoothSurface() {}

    void compute_normals() {
        Vec v0, v1, v2;
        Vec normal;

        vertex_normals = std::vector<Vec>(vertices.size(), Vec(0.0, 0.0, 0.0));
        for (size_t i = 0; i < triangles.size(); i++) {
            const auto & [ idx0, idx1, idx2 ] = triangles[i];
            Vec &v0 = vertices[idx0], &v1 = vertices[idx1], &v2 = vertices[idx2];

            normal = cross(v1 - v0, v2 - v0);
            normal.normalize();

            vertex_normals[idx0] += normal;
            vertex_normals[idx1] += normal;
            vertex_normals[idx2] += normal;
        }

        for (auto &v : vertex_normals) {
            v.normalize();
        }
    }

    void get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                    const double &gamma) const {
        const auto & [ idx0, idx1, idx2 ] = triangles[triangle_idx];
        Vec normal((1.0 - beta - gamma) * vertex_normals[idx0] + beta * vertex_normals[idx1] +
                   gamma * vertex_normals[idx2]);
        normal.normalize();
        hitpoint.normal = normal;
    }
};

#endif
