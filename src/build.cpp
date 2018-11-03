#include "build.hpp"

// Texture
CubicNoise* noise_ptr = new CubicNoise;
WrappedFBmTexture* fbm_texture = new WrappedFBmTexture(noise_ptr, Color(0.25, 0.25, 0.75), 3.0);
MarbleTexture* marble_texture = new MarbleTexture(noise_ptr, Color(0.75, 0.7, 0.7), 0.2, 2.0);

// Material info
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
     Material(new ConstantTexture(Color()), Color(15, 15, 15), ReflectionType::DIFFUSE)},
    {"green",
     Material(new ConstantTexture(Color(0.25, 0.75, 0.25)), Color(), ReflectionType::DIFFUSE)},
    {"checker", Material(new CheckerTexture(Color(1.0, 1.0, 1.0), Color(0.3, 0.3, 0.3)), Color(),
                         ReflectionType::DIFFUSE)},
    {"fbm", Material(fbm_texture, Color(), ReflectionType::DIFFUSE)},
    {"marble", Material(marble_texture, Color(), ReflectionType::DIFFUSE)}};

// Cornel box scene
bool build_1(Scene& scene) {
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

    // Light source
    sphere_ptr = new Sphere(15.0, Vec(50.0, 80.0, 81.6), &materials["light"]);
    scene.add_object(sphere_ptr);

    return true;
}

// Sphere on plane
bool build_2(Scene& scene) {
    Vec eye(10, 5, 9), lookat(0.0, 2.0, 0.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));
    Texture* ibl_ptr(new ConstantTexture(Color(BLACK)));

    scene.set_camera(pinhole_ptr);
    scene.set_ibl(ibl_ptr);

    Object* plane_ptr(new Plane(Vec(0.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0), &materials["marble"]));
    scene.add_object(plane_ptr);

    Object* box_ptr(new Box(Vec(-1.0, 2.0, -1.0), Vec(1.0, 0.0, 1.0), &materials["green"]));
    scene.add_object(box_ptr);

    Object* sphere_ptr(new Sphere(1.0, Vec(0.0, 3.0, 0.0), &materials["red"]));
    scene.add_object(sphere_ptr);

    // Light source
    sphere_ptr = new Sphere(1.0, Vec(5.0, 5.0, 0.0), &materials["week_light_1"]);
    scene.add_object(sphere_ptr);

    return true;
}

// ply file scene
bool build_3(Scene& scene) {
    Vec eye(0.0, 2., 5), lookat(0.0, 1.0, 0.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));

    Texture* ibl_ptr(new ConstantTexture(Color(BLACK)));

    scene.set_camera(pinhole_ptr);
    scene.set_ibl(ibl_ptr);

    Object* plane_ptr(new Plane(Vec(0.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0), &materials["green"]));
    scene.add_object(plane_ptr);

    SmoothSurface* surface = new SmoothSurface(&materials["gray"]);
    if (!load_ply_file("./models/bunny/reconstruction/bun_zipper.ply", surface)) return false;
    surface->scale(2. / (surface->bbox.corner1.x - surface->bbox.corner0.x));
    surface->move(Vec(-surface->bbox.center.x, -surface->bbox.corner0.y, -surface->bbox.center.z));
    Object* surface_ptr = static_cast<Object*>(surface);
    scene.add_object(surface_ptr);

    // Light source
    Object* sphere_ptr;
    sphere_ptr = new Sphere(1.0, Vec(-5.2, 5.0, 2.0), &materials["week_light_1"]);
    scene.add_object(sphere_ptr);
    sphere_ptr = new Sphere(1.0, Vec(5.0, 5.0, 5.0), &materials["week_light_2"]);
    scene.add_object(sphere_ptr);

    UniformRealGenerator rnd(4);
    const int n_lights = 100;
    for (int i = 0; i < n_lights; i++) {
        Material* material =
            new Material(new ConstantTexture(Color()), Color(6 * rnd(), 6 * rnd(), 6 * rnd()),
                         ReflectionType::DIFFUSE);
        sphere_ptr =
            new Sphere(0.1, Vec(10.0 * rnd() - 5., 10.0 * rnd(), 10.0 * rnd() - 5.), material);
        scene.add_object(sphere_ptr);
    }

    return true;
}

// hdr file scene
bool build_4(Scene& scene) {
    Vec eye(-1.8, 1.2, 1.8), lookat(0.0, 0.8, 0.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));

    // IBL
    Image* image_ptr = new Image();
    load_hdr_image("hdr_image/Tokyo_BigSight_3k.hdr", *image_ptr);
    image_ptr->flip();
    Mapping* mapping_ptr = new SphericalMap(45.);
    Texture* ibl_ptr(new ImageTexture(image_ptr, mapping_ptr));

    scene.set_camera(pinhole_ptr);
    scene.set_ibl(ibl_ptr);

    Object* box_ptr(new Box(Vec(-3.0, -0.1, -3.0), Vec(3.0, 0.0, 3.0), &materials["checker"]));
    scene.add_object(box_ptr);

    SmoothSurface* surface = new SmoothSurface(&materials["specular"]);
    if (!load_ply_file("./models/dragon_recon/dragon_vrip_res2.ply", surface)) return false;
    surface->scale(2. / (surface->bbox.corner1.x - surface->bbox.corner0.x));
    surface->move(Vec(-surface->bbox.center.x, -surface->bbox.corner0.y, -surface->bbox.center.z));
    Object* surface_ptr = static_cast<Object*>(surface);
    scene.add_object(surface_ptr);

    return true;
}

// obj file scene
bool build_5(Scene& scene) {
    Vec eye(4.0, 1.2, 0.3), lookat(0.0, 2.8, 0.0);
    Camera* pinhole_ptr(new Pinhole(eye, lookat, 1.0));

    // IBL
    Image* image_ptr = new Image();
    load_hdr_image("hdr_image/MonValley_G_DirtRoad_3k.hdr", *image_ptr);
    image_ptr->flip();
    Mapping* mapping_ptr = new SphericalMap(45.);
    Texture* ibl_ptr(new ImageTexture(image_ptr, mapping_ptr));
    // Texture* ibl_ptr(new ConstantTexture(Color(BLACK)));

    scene.set_camera(pinhole_ptr);
    scene.set_ibl(ibl_ptr);

    std::vector<Object*> tmp_objects;
    if (!load_obj_file("./models/sponza/sponza.obj", tmp_objects)) return false;
    for (Object* obj_ptr : tmp_objects) {
        scene.add_object(obj_ptr);
    };

    // Object* sphere_ptr;
    // sphere_ptr = new Sphere(1.0, Vec(-1.2, 10.0, 1.0), &materials["week_light_1"]);
    // scene.add_object(sphere_ptr);

    return true;
}
