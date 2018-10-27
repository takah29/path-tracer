#ifndef _SCENE_H_
#define _SCENE_H_

#include <iostream>
#include <memory>
#include <vector>
#include "camera.hpp"
#include "mapping.hpp"
#include "object.hpp"
#include "random.hpp"
#include "surface.hpp"
#include "tracer.hpp"

struct ViewPlane {
    double plane_width, plane_height;
    int width_res, height_res;
    double pixel_size;

    ViewPlane(double plane_width, int width_res, int height_res);
};

class Scene {
    Camera *camera_ptr;
    std::vector<Object *> objects;
    BVH bvh;
    Texture *ibl_ptr;

   public:
    Scene();
    ~Scene();

    void set_camera(Camera *camera_ptr);
    void set_ibl(Texture *ibl_ptr);
    void add_object(Object *object_ptr);

    std::vector<Object *> get_lights();
    void construct();
    std::vector<int> traverse(const Ray &ray);
    void render(const std::map<std::string, std::string> &params);
};

#endif
