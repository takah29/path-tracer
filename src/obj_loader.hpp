#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include <unordered_map>
#include <utility>
#include <vector>
#include "surface.hpp"
#include "vec.hpp"

std::tuple<int, int, int> to_vertex_numbers(std::string s);

struct FaceGroup {
    Material *material_ptr;
    std::vector<std::tuple<int, int, int>> triangles;
    std::vector<std::tuple<int, int, int>> triangle_uv_coordinates;
    std::vector<std::tuple<int, int, int>> vertex_normals;
    bool smooth_flag;
    bool texture_flag;

    FaceGroup();
};

class ObjLoader {
    std::vector<Vec> vertices;
    std::vector<std::pair<double, double>> uv_coordinates;
    std::vector<Vec> normals;
    std::vector<FaceGroup> groups;
    std::unordered_map<std::string, Material> materials;

    inline int to_index(const int number, const int n_vertices) {
        return number < 0 ? n_vertices + number : number - 1;
    }

    Surface *face_group_to_surface(const FaceGroup &face_group);

   public:
    ObjLoader();
    ObjLoader(const std::string file_path);

    void all_smooth_flag(const bool smooth_flag);
    bool load_objmtl_file(const std::string file_path);
    bool load_obj_file(const std::string file_path);
    std::unordered_map<std::string, Material> get_materials();
    std::vector<Surface *> convert_to_surfaces();
    void print_obj_data();
};

#endif
