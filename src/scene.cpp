#include "scene.hpp"

ViewPlane::ViewPlane(double plane_width, int width_res, int height_res)
    : plane_width(plane_width), width_res(width_res), height_res(height_res) {
    plane_height = plane_width * height_res / width_res;
    pixel_size = plane_width / width_res;
}

Scene::Scene() : camera_ptr(nullptr), objects(0), ibl_ptr(nullptr) {}
Scene::~Scene() {
    delete camera_ptr;
    delete ibl_ptr;
}

void Scene::set_camera(Camera *camera_ptr) { this->camera_ptr = camera_ptr; }
void Scene::set_ibl(Texture *ibl_ptr) { this->ibl_ptr = ibl_ptr; }
void Scene::add_object(Object *object_ptr) { objects.push_back(object_ptr); }

std::vector<Object *> Scene::get_lights() {
    std::vector<Object *> lights;
    for (const auto &obj : this->objects) {
        if (obj->material_ptr->emission > Vec(0.0, 0.0, 0.0)) {
            lights.push_back(obj);
        }
    }
    return lights;
}

void Scene::construct() {
    std::vector<BBox *> bboxes(objects.size());
    for (size_t i = 0; i < objects.size(); i++) {
        bboxes[i] = &objects[i]->bbox;
    }
    bvh.construct(bboxes);
}

std::vector<int> Scene::traverse(const Ray &ray) { return bvh.traverse(ray); }

void Scene::render(const std::map<std::string, std::string> &params) {
    const int samples = std::stoi(params.at("samples"));
    const int super_samples = std::stoi(params.at("super_samples"));
    const ViewPlane vp(std::stod(params.at("plane_width")), std::stoi(params.at("width_res")),
                       std::stoi(params.at("height_res")));

    Image image(vp.width_res, vp.height_res);

    // for path-tracing
    UniformRealGenerator rnd(1);

    // for ray-tracing or path-tracing with NEE
    std::vector<Object *> lights = get_lights();

    // 空間データ構造の構築
    construct();

#pragma omp parallel for schedule(dynamic, 1) num_threads(8)
    for (int r = 0; r < vp.height_res; r++) {
        if (r % 10 == 0) {
            std::cout << "processing line " << r << std::endl;
        }

        for (int c = 0; c < vp.width_res; c++) {
            int idx = r * vp.width_res + c;

            for (int i = 0; i < super_samples; i++) {
                for (int j = 0; j < super_samples; j++) {
                    Color accumulated_radiance;

                    Point2D pp(
                        vp.pixel_size * (c - 0.5 * vp.width_res + (j + 0.5) / super_samples),
                        vp.pixel_size * (0.5 * vp.height_res - r - 1 + (i + 0.5) / super_samples));
                    Ray ray(camera_ptr->eye, camera_ptr->ray_direction(pp));
                    for (int k = 0; k < samples; k++) {
                        accumulated_radiance += path_trace(ray, objects, bvh, ibl_ptr, rnd, 0);
                        // accumulated_radiance += ray_trace(ray, objects, lights, bvh);
                    }
                    image.color_vec[idx] +=
                        accumulated_radiance / (samples * super_samples * super_samples);
                }
            }
        }
    }
    save_ppm_file("result.ppm", image);
}
