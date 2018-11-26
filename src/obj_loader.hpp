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
    std::unordered_map<std::string, Material*> materials;

    inline int to_index(const int number, const int n_vertices) {
        return number < 0 ? n_vertices + number : number - 1;
    }

    Surface *face_group_to_surface(const FaceGroup &face_group);

    template<typename T>
    void convert(const std::vector<T> &from_vertices, 
                 const std::vector<std::tuple<int, int, int>> &from_indices, 
                 std::vector<T> &to_vertices, 
                 std::vector<std::tuple<int, int, int>> &to_indices);
    
    void push_indices(std::vector<int> &from_indices, 
                      std::vector<std::tuple<int, int, int>> &to_indices, 
                      const std::size_t offset);

    void print_obj_data();
    std::vector<Surface *> convert_to_surfaces();

   public:
    ObjLoader();

    void all_smooth_flag(const bool smooth_flag);
    bool load_objmtl_file(const std::string file_path);
    std::vector<Surface *> load_obj_file(const std::string file_path);
};

#endif
