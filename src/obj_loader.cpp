#include "obj_loader.hpp"
#include <array>
#include <cassert>
#include <unordered_map>
#include "wrapper.hpp"

FaceGroup::FaceGroup()
    : material_ptr(nullptr),
      triangles(0),
      triangle_uv_coordinates(0),
      vertex_normals(0),
      smooth_flag(false) {}

ObjLoader::ObjLoader() : vertices(0), uv_coordinates(0), normals(0), groups(0), materials() {}

void ObjLoader::all_smooth_flag(const bool smooth_flag) {
    for (auto& group : groups) {
        group.smooth_flag = smooth_flag;
    }
}

template<typename T>
void ObjLoader::convert(const std::vector<T> &from_vertices, 
                        const std::vector<std::tuple<int, int, int>> &from_indices, 
                        std::vector<T> &to_vertices, 
                        std::vector<std::tuple<int, int, int>> &to_indices) {
    std::unordered_map<int, int> index_table;

    for (const auto &[v1, v2, v3] : from_indices) {
        for (auto vi : {v1, v2, v3}) {
            if (index_table.find(vi) == index_table.end()) {
                to_vertices.push_back(from_vertices[vi]);
            }
            index_table.emplace(vi, index_table.size());
        }
        to_indices.emplace_back(index_table[v1], index_table[v2], index_table[v3]);
    }
}


Surface* ObjLoader::face_group_to_surface(const FaceGroup& face_group) {
    Surface* const surface = face_group.smooth_flag ? (Surface *)(new SmoothSurface) : (Surface *)(new FlatSurface);

    convert<>(vertices, face_group.triangles, surface->vertices, surface->triangles);
    convert<>(uv_coordinates, face_group.triangle_uv_coordinates, surface->uv_coordinates, surface->triangle_uv_coordinates);

    surface->material_ptr = face_group.material_ptr;

    surface->compute_normals();
    surface->compute_bboxes();
    surface->construct();

    return surface;
}

std::vector<Surface*> ObjLoader::convert_to_surfaces() {
    printf("Converting to surface...\n");
    std::vector<Surface*> surface_objects;
    for (auto idx : groups) {
        surface_objects.push_back(face_group_to_surface(idx));
    }
    return surface_objects;
}

bool ObjLoader::load_objmtl_file(const std::string file_path) {
    std::ifstream infile(file_path);
    if (infile.fail()) {
        printf("Failed to open mtl file.\n");
        return false;
    }

    std::string line;

    std::string keyword;
    std::vector<std::string> sep_s;
    std::string material_name;
    double val0, val1, val2;
    Texture* texture_ptr;
    while (!infile.eof()) {
        getline(infile, line);
        line = strip(line);
        if (line == "" || line[0] == '#') continue;

        sep_s = split_reg(line, " +");
        if (sep_s[0] == "newmtl") {
            material_name = sep_s[1];
            materials.emplace(material_name,
                              new Material(new ConstantTexture(GRAY), BLACK, ReflectionType::DIFFUSE));
        } else if (sep_s[0] == "Kd") {
            sscanf(line.data(), "Kd %lf %lf %lf", &val0, &val1, &val2);
            texture_ptr = new ConstantTexture(Color(val0, val1, val2));
            materials[material_name]->color_ptr = texture_ptr;
        } else if (sep_s[0] == "map_Kd") {
            int pos = file_path.find_last_of("/") + 1;
            std::string image_path = file_path.substr(0, pos) + sep_s[1];
            Image* image_ptr = new Image();
            load_rgb_image_file(image_path, *image_ptr);
            texture_ptr = new ImageTexture(image_ptr, nullptr);
            materials[material_name]->color_ptr = texture_ptr;
            materials[material_name]->texture_flag = true;
        }
    }
    return true;
}

std::tuple<int, int, int> to_vertex_numbers(std::string s) {
    auto vec_str = split(s, '/');

    switch (vec_str.size()) {
        case 1: {
            const int v = std::stoi(vec_str[0]);
            return {v, 0, 0};
        }
        case 2: {
            const int v = std::stoi(vec_str[0]);
            const int vt = std::stoi(vec_str[1]);
            return {v, vt, 0};
        }
        case 3: {
            const int v = std::stoi(vec_str[0]);
            const int vt = vec_str[1] != "" ? std::stoi(vec_str[1]) : 0;
            const int vn = std::stoi(vec_str[2]);
            return {v, vt, vn};
        }
        default:
            assert(!" 1 <= vec_str.size() <=3 expected.");
            return {0, 0, 0};
    }
}

void ObjLoader::push_indices(std::vector<int> &from_indices, 
                             std::vector<std::tuple<int, int, int>> &to_indices, 
                             const std::size_t offset) {

    const auto no_index = from_indices.size();
    std::transform(from_indices.cbegin(), from_indices.cend(), from_indices.begin(), [&](auto v){ return to_index(v, offset); });
    for (std::size_t i = 0; i < no_index - 2; i++) {
        to_indices.emplace_back(from_indices[0], from_indices[i + 1], from_indices[i + 2]);
    }
}

std::vector<Surface*> ObjLoader::load_obj_file(const std::string file_path) {
    print("Loading obj file...");
    std::ifstream infile(file_path);
    if (infile.fail()) {
        printf("Failed to open obj file.\n");
        return std::vector<Surface*>();
    }

    FaceGroup group;
    std::string prev_keyword;

    auto load_real3 = [&](const std::string line, const std::string format) {
        double val0, val1, val2;
        sscanf(line.c_str(), format.c_str(), &val0, &val1, &val2);
        return std::make_tuple(val0, val1, val2);
    };

    while (!infile.eof()) {
        std::string line;
        getline(infile, line);
        line = strip(line);

        const std::vector<std::string> sep_s = split_reg(line, " +");
        std::string keyword;
        if (line[0] == '#' || line == "") {
            keyword = "none";
        } else {
            keyword = sep_s[0];
        }

        if (keyword == "mtllib") {
            int pos = file_path.find_last_of("/") + 1;
            std::string mtl_path = file_path.substr(0, pos) + sep_s[1];
            if (!load_objmtl_file(mtl_path)) {
                printf("mtl file not found.");
                return std::vector<Surface*>();
            }
        } else if (keyword == "o" || keyword == "g") {
            group = FaceGroup();
        } else if (keyword == "v") {
            auto [val0, val1, val2] = load_real3(line, "v %lf %lf %lf");
            vertices.emplace_back(val0, val1, val2);
        } else if (keyword == "vt") {
            auto [val0, val1, val2] = load_real3(line, "vt %lf %lf %lf");
            uv_coordinates.emplace_back(val0, val1);
        } else if (keyword == "vn") {
            auto [val0, val1, val2] = load_real3(line, "vn %lf %lf %lf");
            normals.emplace_back(val0, val1, val2);
        } else if (keyword == "usemtl") {
            group.material_ptr = materials.at(sep_s[1]);
        } else if (keyword == "s") {
            print(line);
            if (sep_s[1] == "1") {
                group.smooth_flag = true;
            } else if (sep_s[1] == "off") {
                group.smooth_flag = false;
            }
        } else if (keyword == "f") {
            const std::size_t no_index = sep_s.size() - 1;
            std::vector<int> v_idxs(no_index), vt_idxs(no_index), vn_idxs(no_index);

            for (std::size_t i = 0; i < no_index; i++) {
                std::tie(v_idxs[i], vt_idxs[i], vn_idxs[i]) = to_vertex_numbers(sep_s[i + 1]);
            }
            
            push_indices(v_idxs, group.triangles, vertices.size());

            if (vt_idxs[0] != 0) {
                push_indices(vt_idxs, group.triangle_uv_coordinates, uv_coordinates.size());
            }
            if (vn_idxs[0] != 0) {
                push_indices(vn_idxs, group.vertex_normals, normals.size());
            }
        } else if (prev_keyword == "f" && keyword != "f") {
            groups.push_back(group);
        }
        prev_keyword = keyword;
    }
    print_obj_data();
    return convert_to_surfaces();
}

void ObjLoader::print_obj_data() {
    printf("----- OBJ Data Info -----\n");
    printf("Number of Vertices      : %d\n", static_cast<int>(vertices.size()));
    printf("Number of UV Coordinates: %d\n", static_cast<int>(uv_coordinates.size()));
    printf("Number of Normals       : %d\n", static_cast<int>(normals.size()));
    printf("Number of Face Groups   : %d\n", static_cast<int>(groups.size()));
    printf("Number of Matrials      : %d\n", static_cast<int>(materials.size()));

    print("----- Face Group Info -----");
    for (size_t i = 0; i < groups.size(); i++) {
        printf("Group %2d: Triangles %6d, UV_Points %6d, Vertex_Normals %6d\n",
               static_cast<int>(i + 1), static_cast<int>(groups[i].triangles.size()),
               static_cast<int>(groups[i].triangle_uv_coordinates.size()),
               static_cast<int>(groups[i].vertex_normals.size()));
    }
    print("-------------------------\n");
}
