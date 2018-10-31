#include "surface.hpp"

Surface::Surface() : Object(), vertices(0), triangles(0), triangle_bboxes(0) {}
Surface::Surface(Material *material_ptr)
    : Object(Vec(0.0, 0.0, 0.0), BBox(), material_ptr),
      vertices(0),
      triangles(0),
      triangle_bboxes(0) {
    this->bbox.empty();
}
Surface::~Surface() {}

void Surface::scale(const double x) {
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

void Surface::move(const Vec x) {
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

void Surface::set_material(Material *material_ptr) { this->material_ptr = material_ptr; }

void Surface::compute_bboxes() {
    double min_x = INF, min_y = INF, min_z = INF;
    double max_x = -INF, max_y = -INF, max_z = -INF;
    for (size_t i = 0; i < triangles.size(); i++) {
        const auto & [ idx0, idx1, idx2 ] = triangles[i];
        const Vec &v0(vertices[idx0]), &v1(vertices[idx1]), &v2(vertices[idx2]);
        BBox bbox(min(min(v0, v1), v2) - EPS, max(max(v0, v1), v2) + EPS);
        triangle_bboxes.push_back(bbox);
        if (bbox.corner0.x < min_x) min_x = bbox.corner0.x;
        if (bbox.corner0.y < min_y) min_y = bbox.corner0.y;
        if (bbox.corner0.z < min_z) min_z = bbox.corner0.z;
        if (bbox.corner1.x > max_x) max_x = bbox.corner1.x;
        if (bbox.corner1.y > max_y) max_y = bbox.corner1.y;
        if (bbox.corner1.z > max_z) max_z = bbox.corner1.z;
    }
    Vec corner0(min_x - EPS, min_y - EPS, min_z - EPS);
    Vec corner1(max_x + EPS, max_y + EPS, max_z + EPS);
    this->bbox = BBox(corner0, corner1);
    this->center = (corner0 + corner1) / 2.0;
}

void Surface::construct() {
    std::vector<BBox *> bboxes(triangle_bboxes.size());
    for (size_t i = 0; i < triangle_bboxes.size(); i++) {
        bboxes[i] = &triangle_bboxes[i];
    }
    bvh.construct(bboxes);
}

std::vector<int> Surface::traverse(const Ray &ray) { return bvh.traverse(ray); }

bool Surface::intersect_triangle(const int &triangle_idx, const Ray &ray,
                                 Hitpoint &hitpoint) const {
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

bool Surface::intersect(const Ray &ray, Hitpoint &hitpoint) const {
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

FlatSurface::FlatSurface() : Surface(), triangle_normals(0) {}
FlatSurface::FlatSurface(Material *material_ptr) : Surface(material_ptr), triangle_normals(0) {
    this->bbox.empty();
}

FlatSurface::~FlatSurface() {}

void FlatSurface::compute_normals() {
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

void FlatSurface::get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                             const double &gamma) const {
    hitpoint.normal = triangle_normals[triangle_idx];
}

SmoothSurface::SmoothSurface() : Surface(), vertex_normals(0) {}
SmoothSurface::SmoothSurface(Material *material_ptr) : Surface(material_ptr), vertex_normals(0) {
    this->bbox.empty();
}

SmoothSurface::~SmoothSurface() {}

void SmoothSurface::compute_normals() {
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
        // 不正な法線を置き換え
        if (v == Vec(0.0, 0.0, 0.0)) v = Vec(1.0, 0.0, 0.0);
        v.normalize();
    }
}

void SmoothSurface::get_normal(const int triangle_idx, Hitpoint &hitpoint, const double &beta,
                               const double &gamma) const {
    const auto & [ idx0, idx1, idx2 ] = triangles[triangle_idx];
    Vec normal((1.0 - beta - gamma) * vertex_normals[idx0] + beta * vertex_normals[idx1] +
               gamma * vertex_normals[idx2]);
    normal.normalize();
    hitpoint.normal = normal;
}
