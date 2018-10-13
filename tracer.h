#ifndef _TRACER_H_
#define _TRACER_H_

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "bvh.h"
#include "constant.h"
#include "material.h"
#include "object.h"
#include "random.h"
#include "vec.h"

const Color BACKGROUND_COLOR = BLACK;
const double AMBIENT_COEF = 0.05;  // 環境光
const int DEPTH = 4;
const int DEPTH_LIMIT = 16;
const double IOR = 1.5;

// シーンの全オブジェクトの内、レイの原点に最も近いオブジェクトの情報を取得する
bool intersect_objects(const Ray &ray, const std::vector<Object *> objects, const BVH &bvh,
                       Intersection &intersection) {
    intersection.hitpoint.distance = INF;
    intersection.object_id = -1;

    for (const size_t idx : bvh.traverse(ray)) {
        Hitpoint hitpoint;
        if (objects[idx]->intersect(ray, hitpoint)) {
            if (hitpoint.distance < intersection.hitpoint.distance) {
                intersection.hitpoint = hitpoint;
                intersection.object_id = idx;
            }
        }
    }

    return (intersection.object_id != -1);
}

// 正規直交基底を計算
// 入力wは単位ベクトルを仮定
inline std::pair<Vec, Vec> create_orthonormal_basis(const Vec w) {
    Vec u;
    if (fabs(w.x) > EPS) {
        u = normalize(cross(Vec(0.0, 1.0, 0.0), w));
    } else {
        u = normalize(cross(Vec(1.0, 0.0, 0.0), w));
    }
    Vec v = cross(w, u);
    return std::make_pair(u, v);
}

// 半球面からサンプリングする
inline Vec sample_from_hemisphere(const Vec &u, const Vec &v, const Vec &w,
                                  UniformRealGenerator &rnd, const double e) {
    const double phi = 2 * M_PI * rnd();
    const double cos_phi = std::cos(phi);
    const double sin_phi = std::sin(phi);
    const double cos_theta = std::pow(rnd(), 1.0 / (e + 1.0));
    const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    return sin_theta * cos_phi * u + sin_theta * sin_phi * v + cos_theta * w;
}

// 鏡面反射時のレイを求める
inline Ray calc_reflection_ray(const Vec &dir, const Hitpoint &hitpoint) {
    return Ray(hitpoint.position, dir - hitpoint.normal * 2.0 * dot(hitpoint.normal, dir));
}

// レイが交差したオブジェクトの色を取得する（デバッグ用）
Color trace_for_debug(const Ray &ray, const std::vector<Object *> objects, const BVH &bvh) {
    Intersection intersection;
    if (!intersect_objects(ray, objects, bvh, intersection)) {
        return BACKGROUND_COLOR;
    }

    return objects[intersection.object_id]->material_ptr->color_ptr->get_value(intersection.hitpoint);
}

// レイトレーサー（シーン確認用）
Color ray_trace(const Ray &ray, const std::vector<Object *> objects,
                const std::vector<Object *> lights, const BVH &bvh) {
    Intersection intersection;
    if (!intersect_objects(ray, objects, bvh, intersection)) {
        return BACKGROUND_COLOR;
    }

    const Object *target_object_ptr = objects[intersection.object_id];
    const Hitpoint &hitpoint = intersection.hitpoint;
    const Vec orienting_normal =
        dot(hitpoint.normal, ray.dir) < 0.0 ? hitpoint.normal : (-1.0 * hitpoint.normal);
    const Color &target_obj_color = target_object_ptr->material_ptr->color_ptr->get_value(hitpoint);
    const Color &target_obj_emission = target_object_ptr->material_ptr->emission;

    Color color(0.0, 0.0, 0.0);
    Ray shadow_ray;

    for (const auto &light_ptr : lights) {
        Hitpoint tmp_hp;

        const Vec d(light_ptr->center - hitpoint.position);
        shadow_ray.org = hitpoint.position + EPS * d;
        shadow_ray.dir = normalize(d);
        const double light_dist = norm(d) - EPS;

        for (const size_t idx : bvh.traverse(shadow_ray)) {
            Object *obj_ptr = objects[idx];
            if (light_ptr != obj_ptr) {
                obj_ptr->intersect(shadow_ray, tmp_hp);
                const Color &light_emission = light_ptr->material_ptr->emission;
                if (tmp_hp.distance >= light_dist) {
                    color += target_obj_emission +
                             std::max(dot(normalize(light_ptr->center - hitpoint.position),
                                          orienting_normal),
                                      0.0) *
                                 multiply(light_emission, target_obj_color) / 360.0;
                }
            }
        }
    }
    color += target_obj_color * AMBIENT_COEF;
    return color;
}

// パストレーサー
Color path_trace(const Ray &ray, const std::vector<Object *> objects, const BVH &bvh,
                 UniformRealGenerator &rnd, const int depth) {
    Intersection intersection;
    if (!intersect_objects(ray, objects, bvh, intersection)) {
        return BACKGROUND_COLOR;
    }

    const Object *target_object = objects[intersection.object_id];
    const Hitpoint &hitpoint = intersection.hitpoint;
    const Vec orienting_normal =
        dot(hitpoint.normal, ray.dir) < 0.0 ? hitpoint.normal : (-1.0 * hitpoint.normal);
    const Color &target_obj_color = target_object->material_ptr->color_ptr->get_value(hitpoint);
    const Color &target_obj_emission = target_object->material_ptr->emission;

    double russian_roulette_probability =
        std::max({target_obj_color.x, target_obj_color.y, target_obj_color.z});

    if (depth > DEPTH_LIMIT) {
        russian_roulette_probability *= std::pow(0.5, depth - DEPTH_LIMIT);
    }

    if (depth > DEPTH) {
        if (rnd() > russian_roulette_probability) {
            return target_obj_emission;
        }
    } else {
        russian_roulette_probability = 1.0;
    }

    Color incoming_radiance;
    Color weight;

    switch (target_object->material_ptr->reflection_type) {
        case ReflectionType::DIFFUSE: {
            const Vec &w = orienting_normal;
            auto[u, v] = create_orthonormal_basis(w);
            Vec dir = sample_from_hemisphere(u, v, w, rnd, 1.0);
            incoming_radiance =
                path_trace(Ray(hitpoint.position, dir), objects, bvh, rnd, depth + 1);
            weight = target_obj_color / russian_roulette_probability;
        } break;

        case ReflectionType::SPECULAR: {
            Ray reflection_ray = calc_reflection_ray(ray.dir, hitpoint);
            incoming_radiance = path_trace(reflection_ray, objects, bvh, rnd, depth + 1);
            weight = target_obj_color / russian_roulette_probability;
        } break;

        case ReflectionType::REFRACTION: {
            Ray reflection_ray = calc_reflection_ray(ray.dir, hitpoint);
            const bool into = dot(hitpoint.normal, orienting_normal) > 0.0;

            const double n1 = 1.0, n2 = IOR;
            const double n1n2 = into ? n1 / n2 : n2 / n1;
            const double ans_dot = dot(ray.dir, orienting_normal);
            const double cos2t = 1.0 - n1n2 * n1n2 * (1.0 - ans_dot * ans_dot);

            if (cos2t < 0.0) {
                incoming_radiance = path_trace(reflection_ray, objects, bvh, rnd, depth + 1);
                weight = target_obj_color / russian_roulette_probability;
                break;
            }

            const Ray refraction_ray(
                hitpoint.position,
                normalize(ray.dir * n1n2 - hitpoint.normal * (into ? 1.0 : -1.0) *
                                               (ans_dot * n1n2 + std::sqrt(cos2t))));

            const double a = n2 - n1, b = n2 + n1;
            const double R0 = (a * a) / (b * b);
            const double c =
                1.0 - (into ? -ans_dot : dot(refraction_ray.dir, -1.0 * orienting_normal));
            const double Re = R0 + (1.0 - R0) * std::pow(c, 5.0);
            const double n1n2_2 = std::pow(n1n2, 2.0);
            const double Tr = (1.0 - Re) * n1n2_2;

            const double probability = 0.25 + 0.5 * Re;
            if (depth > 2) {
                if (rnd() < probability) {  // 反射
                    incoming_radiance =
                        path_trace(reflection_ray, objects, bvh, rnd, depth + 1) * Re;
                    weight = target_obj_color / (probability * russian_roulette_probability);
                } else {  // 屈折
                    incoming_radiance =
                        path_trace(refraction_ray, objects, bvh, rnd, depth + 1) * Tr;
                    weight =
                        target_obj_color / ((1.0 - probability) * russian_roulette_probability);
                }
            } else {  // 屈折と反射の両方を追跡
                incoming_radiance = path_trace(reflection_ray, objects, bvh, rnd, depth + 1) * Re +
                                    path_trace(refraction_ray, objects, bvh, rnd, depth + 1) * Tr;
                weight = target_obj_color / russian_roulette_probability;
            }
        } break;
    }

    return target_obj_emission + multiply(weight, incoming_radiance);
}

#endif
