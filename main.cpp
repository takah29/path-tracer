#include <map>
#include <memory>
#include <random>
#include <string>
#include "camera.h"
#include "loader.h"
#include "random.h"
#include "scene.h"
#include "tracer.h"

bool build_cornelbox(Scene& scene);
bool build_sphere_plane(Scene& scene);
bool build_from_plyfile(Scene& scene);

int main() {
    std::map<std::string, std::string> params{{"samples", "1"},
                                              {"super_samples", "2"},
                                              {"plane_width", "1.0"},
                                              {"width_res", "320"},
                                              {"height_res", "240"}};

    Scene scene;
    if (!build_from_plyfile(scene)) return 0;
    scene.render(params);
}

// マテリアル情報
std::map<std::string, Material> materials = {
    {"red",
     Material(new ConstantTexture(Color(0.75, 0.25, 0.25)), Color(), ReflectionType::DIFFUSE)},
    {"blue",
     Material(new ConstantTexture(Color(0.25, 0.25, 0.75)), Color(), ReflectionType::DIFFUSE)},
    {"gray",
     Material(new ConstantTexture(Color(0.75, 0.75, 0.75)), Color(), ReflectionType::DIFFUSE)},
    {"black", Material(new ConstantTexture(Color()), Color(), ReflectionType::DIFFUSE)},
    {"cyan",
     Material(new ConstantTexture(Color(0.25, 0.75, 0.75)), Color(), ReflectionType::DIFFUSE)},
    {"specular",
     Material(new ConstantTexture(Color(0.99, 0.99, 0.99)), Color(), ReflectionType::SPECULAR)},
    {"refraction",
     Material(new ConstantTexture(Color(0.99, 0.99, 0.99)), Color(), ReflectionType::REFRACTION)},
    {"light", Material(new ConstantTexture(Color()), Color(36, 36, 36), ReflectionType::DIFFUSE)},
    {"week_light_1",
     Material(new ConstantTexture(Color()), Color(25, 25, 25), ReflectionType::DIFFUSE)},
    {"week_light_2",
     Material(new ConstantTexture(Color()), Color(5, 5, 5), ReflectionType::DIFFUSE)},
    {"green",
     Material(new ConstantTexture(Color(0.25, 0.75, 0.25)), Color(), ReflectionType::DIFFUSE)},
    {"checker", Material(new CheckerTexture(Color(1.0, 1.0, 1.0), Color(0.0, 0.0, 0.0)), Color(),
                         ReflectionType::DIFFUSE)}};

// コーネルボックス
bool build_cornelbox(Scene& scene) {
    Vec eye(50, 50, 220), lookat(50.0, 30.0, -1.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));
    scene.set_camera(pinhole_ptr);

    Object* sphere_ptr(new Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), &materials["red"]));
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), &materials["blue"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1e5, Vec(50, 40.8, 1e5), &materials["gray"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1e5, Vec(50, 40.8, -1e5 + 250), &materials["black"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1e5, Vec(50, 1e5, 81.6), &materials["gray"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), &materials["gray"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(20, Vec(65, 20, 20), &materials["cyan"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(16.5, Vec(27, 16.5, 47), &materials["specular"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(16.5, Vec(77, 16.5, 78), &materials["refraction"]);
    scene.add_object(sphere_ptr);

    // 光源
    sphere_ptr = new Sphere(15.0, Vec(50.0, 80.0, 81.6), &materials["light"]);
    scene.add_object(sphere_ptr);

    return true;
}

// 平面上に球
bool build_sphere_plane(Scene& scene) {
    Vec eye(10, 5, 10), lookat(0.0, 2.0, 0.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));
    scene.set_camera(pinhole_ptr);

    Object* plane_ptr(new Plane(Vec(0.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0), &materials["red"]));
    scene.add_object(plane_ptr);

    Object* box_ptr(new Box(Vec(-1.0, 2.0, -1.0), Vec(1.0, 0.0, 1.0), &materials["green"]));
    scene.add_object(box_ptr);

    Object* sphere_ptr(new Sphere(1.0, Vec(0.0, 3.0, 0.0), &materials["specular"]));
    scene.add_object(sphere_ptr);

    // 光源
    sphere_ptr = new Sphere(1.0, Vec(5.0, 5.0, 0.0), &materials["week_light"]);
    scene.add_object(sphere_ptr);

    return true;
}

bool build_from_plyfile(Scene& scene) {
    Vec eye(0, 3, 9), lookat(0.0, 2.0, 0.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));
    scene.set_camera(pinhole_ptr);

    Object* plane_ptr(new Plane(Vec(0.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0), &materials["checker"]));
    scene.add_object(plane_ptr);

    SmoothSurface* surface = new SmoothSurface(&materials["refraction"]);
    if (!from_ply_file("./models/bunny/reconstruction/bun_zipper.ply", surface)) return false;
    surface->scale(2. / (surface->bbox.corner1.x - surface->bbox.corner0.x));
    surface->move(Vec(-surface->bbox.center.x, -surface->bbox.corner0.y, -surface->bbox.center.z));
    Object* surface_ptr = static_cast<Object*>(surface);
    scene.add_object(surface_ptr);

    // 光源
    Object* sphere_ptr;
    sphere_ptr = new Sphere(1.0, Vec(-5.2, 5.0, 2.0), &materials["week_light_1"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1.0, Vec(5.0, 5.0, 5.0), &materials["week_light_2"]);
    scene.add_object(sphere_ptr);

    UniformRealGenerator rnd(4);
    const int n_lights = 0;
    for (int i = 0; i < n_lights; i++) {
        Material material(new ConstantTexture(Color()), Color(6 * rnd(), 6 * rnd(), 6 * rnd()),
                          ReflectionType::DIFFUSE);
        sphere_ptr =
            new Sphere(0.1, Vec(10.0 * rnd() - 5., 10.0 * rnd(), 10.0 * rnd() - 5.), &material);
        scene.add_object(sphere_ptr);
    }

    return true;
}
