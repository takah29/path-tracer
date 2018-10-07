#ifndef _SCENE_H_
#define _SCENE_H_

#include <omp.h>
#include <iostream>
#include <memory>
#include <vector>
#include "camera.h"
#include "object.h"
#include "random.h"
#include "surface.h"
#include "tracer.h"

struct ViewPlane {
    double plane_width, plane_height;
    int width_res, height_res;
    double pixel_size;

    ViewPlane(double plane_width, int width_res, int height_res)
        : plane_width(plane_width), width_res(width_res), height_res(height_res) {
        plane_height = plane_width * height_res / width_res;
        pixel_size = plane_width / width_res;
    }
};

class Scene {
    Camera *camera_ptr;
    std::vector<Object *> objects;
    BVH bvh;

   public:
    Scene() : camera_ptr(nullptr), objects(0) {}

    ~Scene() { delete camera_ptr; }

    void set_camera(Camera *camera_ptr) { this->camera_ptr = camera_ptr; }
    void add_object(Object *object_ptr) { objects.push_back(object_ptr); }

    std::vector<Object *> get_lights() {
        std::vector<Object *> lights;
        for (const auto &obj : this->objects) {
            if (obj->material.emission > Vec(0.0, 0.0, 0.0)) {
                lights.push_back(obj);
            }
        }
        return lights;
    }

    void construct() {
        std::vector<BBox *> bboxes(objects.size());
        for (size_t i = 0; i < objects.size(); i++) {
            bboxes[i] = &objects[i]->bbox;
        }
        bvh.construct(bboxes);
    }

    std::vector<int> traverse(const Ray &ray) { return bvh.traverse(ray); }

    void render(const std::map<std::string, std::string> &params) {
        const int samples = std::stoi(params.at("samples"));
        const int super_samples = std::stoi(params.at("super_samples"));
        const ViewPlane vp(std::stoi(params.at("plane_width")), std::stoi(params.at("width_res")),
                           std::stoi(params.at("height_res")));

        std::vector<Color> img_vector(vp.height_res * vp.width_res, 0.0);

        // path-tracing
        UniformRealGenerator rnd(1);

        // ray-tracing or path-tracing with NEE
        std::vector<Object *> lights = get_lights();

        // 空間データ構造の構築
        construct();

#pragma omp parallel for schedule(dynamic, 1)
        for (int r = 0; r < vp.height_res; r++) {
            if (r % 10) {
                std::cout << "processing line " << r << std::endl;
            }

            for (int c = 0; c < vp.width_res; c++) {
                int idx = r * vp.width_res + c;

                for (int i = 0; i < super_samples; i++) {
                    for (int j = 0; j < super_samples; j++) {
                        Color accumulated_radiance;

                        Point2D pp(
                            vp.pixel_size * (c - 0.5 * vp.width_res + (j + 0.5) / super_samples),
                            vp.pixel_size *
                                (0.5 * vp.height_res - r - 1 + (i + 0.5) / super_samples));
                        Ray ray(camera_ptr->eye, camera_ptr->ray_direction(pp));

                        for (int k = 0; k < samples; k++) {
                            // accumulated_radiance += path_trace(ray, objects, bvh, rnd, 0);
                            accumulated_radiance += ray_trace(ray, objects, lights, bvh);
                        }
                        img_vector[idx] +=
                            accumulated_radiance / (samples * super_samples * super_samples);
                    }
                }
            }
        }
        save_ppm_file("result.ppm", img_vector, vp.width_res, vp.height_res);
    }
};

#endif
