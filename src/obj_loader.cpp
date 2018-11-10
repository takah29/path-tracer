#include "obj_loader.hpp"
#include <unordered_map>

FaceGroup::FaceGroup()
    : material_ptr(nullptr), triangles(0), triangle_uv_coordinates(0), vertex_normals(0) {}

ObjLoader::ObjLoader() : vertices(0), uv_coordinates(0), normals(0), groups(0), materials() {}
ObjLoader::ObjLoader(std::string file_path)
    : vertices(0), uv_coordinates(0), normals(0), groups(0), materials() {
    load_obj_file(file_path);
}

Surface* ObjLoader::face_group_to_surface(const FaceGroup& face_group) {
    Surface* surface = new SmoothSurface;
    std::unordered_map<int, int> index_table;

    for (size_t i = 0; i < face_group.triangles.size(); i++) {
        const auto [idx0, idx1, idx2] = face_group.triangles[i];
        int tmp_idx0 = 0, tmp_idx1 = 0, tmp_idx2 = 0;

        auto p0 = index_table.emplace(idx0, index_table.size());
        if (p0.second) {
            surface->vertices.push_back(vertices[idx0]);
        }
        tmp_idx0 = index_table[idx0];

        auto p1 = index_table.emplace(idx1, index_table.size());
        if (p1.second) {
            surface->vertices.push_back(vertices[idx1]);
        }
        tmp_idx1 = index_table[idx1];

        auto p2 = index_table.emplace(idx2, index_table.size());
        if (p2.second) {
            surface->vertices.push_back(vertices[idx2]);
        }
        tmp_idx2 = index_table[idx2];
        surface->triangles.emplace_back(tmp_idx0, tmp_idx1, tmp_idx2);
    }

    index_table.clear();
    for (size_t i = 0; i < face_group.triangle_uv_coordinates.size(); i++) {
        const auto [idx0, idx1, idx2] = face_group.triangle_uv_coordinates[i];
        int tmp_idx0 = 0, tmp_idx1 = 0, tmp_idx2 = 0;

        auto p0 = index_table.emplace(idx0, index_table.size());
        if (p0.second) {
            surface->uv_coordinates.push_back(uv_coordinates[idx0]);
        }
        tmp_idx0 = index_table[idx0];

        auto p1 = index_table.emplace(idx1, index_table.size());
        if (p1.second) {
            surface->uv_coordinates.push_back(uv_coordinates[idx1]);
        }
        tmp_idx1 = index_table[idx1];

        auto p2 = index_table.emplace(idx2, index_table.size());
        if (p2.second) {
            surface->uv_coordinates.push_back(uv_coordinates[idx2]);
        }
        tmp_idx2 = index_table[idx2];

        surface->triangle_uv_coordinates.emplace_back(tmp_idx0, tmp_idx1, tmp_idx2);
    }

    surface->compute_normals();
    surface->compute_bboxes();
    surface->construct();

    return surface;
}

std::vector<Surface*> ObjLoader::convert_to_surfaces() {
    printf("converting OBJ data structure...\n");
    std::vector<Surface*> surface_objects;
    for (size_t i = 0; i < groups.size(); i++) {
        surface_objects.push_back(face_group_to_surface(groups[i]));
    }
    return surface_objects;
}

// bool ObjLoader::load_objmtl_file(const std::string file_path,
//                                  std::map<std::string, Material>& materials) {
//     print("load mtl file...");
//     std::ifstream infile(file_path);
//     std::string line;

//     std::string keyword;
//     std::vector<std::string> sep_s;
//     std::string material_name;
//     Material material;
//     double val0, val1, val2;
//     Texture* texture_ptr;
//     while (!infile.eof()) {
//         getline(infile, line);
//         line = strip(line);
//         if (line == "" || line[0] == '#') continue;

//         sep_s = split_reg(line, " +");
//         if (sep_s[0] == "newmtl") {
//             material_name = sep_s[1];
//             materials[material_name] = Material();
//         } else if (sep_s[0] == "Kd") {
//             sscanf(line.data(), "Kd %lf %lf %lf", &val0, &val1, &val2);
//             texture_ptr = new ConstantTexture(Color(val0, val1, val2));
//             materials[material_name].color_ptr = texture_ptr;
//         } else if (sep_s[0] == "map_Kd") {
//             int pos = file_path.find_last_of("/") + 1;
//             std::string image_path = file_path.substr(0, pos) + sep_s[1];
//             Image* image_ptr = new Image();
//             load_rgb_image_file(image_path, *image_ptr);
//             texture_ptr = new ImageTexture(image_ptr, nullptr);
//             materials[material_name].color_ptr = texture_ptr;
//         }
//     }
//     return true;
// }

std::tuple<int, int, int> to_vertex_numbers(std::string s) {
    int v = 0, vt = 0, vn = 0;
    auto vec_str = split(s, '/');

    switch (vec_str.size()) {
        case 1: {
            v = std::stoi(vec_str[0]);
            break;
        }
        case 2: {
            v = std::stoi(vec_str[0]);
            vt = std::stoi(vec_str[1]);
            break;
        }
        case 3: {
            v = std::stoi(vec_str[0]);
            vt = vec_str[1] != "" ? std::stoi(vec_str[1]) : 0;
            vn = std::stoi(vec_str[2]);
            break;
        }
    }
    return {v, vt, vn};
}

bool ObjLoader::load_obj_file(const std::string file_path) {
    print("loading obj file...");
    std::ifstream infile(file_path + ".obj");
    std::string line;
    std::vector<std::string> str_vec;

    std::string keyword;
    std::vector<std::string> sep_s;
    FaceGroup group;
    std::string prev_keyword;
    int v0_idx, v1_idx, v2_idx, v3_idx;
    int vt0_idx, vt1_idx, vt2_idx, vt3_idx;
    int vn0_idx, vn1_idx, vn2_idx, vn3_idx;
    double val0, val1, val2;
    // auto materials = new std::map<std::string, Material>;
    int count_obj = 0;
    while (!infile.eof()) {
        getline(infile, line);
        line = strip(line);

        sep_s = split_reg(line, " +");
        if (line[0] == '#' || line == "") {
            keyword = "none";
        } else {
            keyword = sep_s[0];
        }

        if (keyword == "mtllib") {
            // int pos = file_path.find_last_of("/") + 1;
            // std::string mtl_path = file_path.substr(0, pos) + sep_s[1];
            // if (!load_objmtl_file(file_path + "mtl")) {
            //     printf("mtl file not found.");
            //     return false;
            // }
        } else if (keyword == "o" || keyword == "g") {
            group = FaceGroup();
            prev_keyword = keyword;
            count_obj++;
        } else if (keyword == "v") {
            sscanf(line.c_str(), "v  %lf %lf %lf", &val0, &val1, &val2);
            // print("v", val0, val1, val2, "size:", tmp_vertices.size());
            vertices.emplace_back(val0, val1, val2);
            prev_keyword = keyword;
        } else if (keyword == "vt") {
            sscanf(line.c_str(), "vt %lf %lf %lf", &val0, &val1, &val2);
            // print("vt:", val0, val1, val2, "size:", uv_coordinates.size());
            uv_coordinates.emplace_back(val0, val1);
            prev_keyword = keyword;
        } else if (keyword == "vn") {
            sscanf(line.c_str(), "vn %lf %lf %lf", &val0, &val1, &val2);
            // print("vn:", val0, val1, val2);
            normals.emplace_back(val0, val1, val2);
            prev_keyword = keyword;
        } else if (keyword == "usemtl") {
            // Material* mtl_ptr = &materials.at(sep_s[1]);
            // group->material_ptr = mtl_ptr;
        } else if (keyword == "f") {
            if (sep_s.size() == 4) {
                std::tie(v0_idx, vt0_idx, vn0_idx) = to_vertex_numbers(sep_s[1]);
                std::tie(v1_idx, vt1_idx, vn1_idx) = to_vertex_numbers(sep_s[2]);
                std::tie(v2_idx, vt2_idx, vn2_idx) = to_vertex_numbers(sep_s[3]);

                auto n_vertices = vertices.size();
                v0_idx = to_index(v0_idx, n_vertices);
                v1_idx = to_index(v1_idx, n_vertices);
                v2_idx = to_index(v2_idx, n_vertices);
                group.triangles.emplace_back(v0_idx, v1_idx, v2_idx);

                if (vt0_idx != 0) {
                    auto n_uv_coordinates = uv_coordinates.size();
                    vt0_idx = to_index(vt0_idx, n_uv_coordinates);
                    vt1_idx = to_index(vt1_idx, n_uv_coordinates);
                    vt2_idx = to_index(vt2_idx, n_uv_coordinates);
                    group.triangle_uv_coordinates.emplace_back(vt0_idx, vt1_idx, vt2_idx);
                }
                if (vn0_idx != 0) {
                    auto n_normals = normals.size();
                    v0_idx = to_index(v0_idx, n_normals);
                    v1_idx = to_index(v1_idx, n_normals);
                    v2_idx = to_index(v2_idx, n_normals);
                    group.vertex_normals.emplace_back(vt0_idx, vt1_idx, vt2_idx);
                }
            } else if (sep_s.size() == 5) {  // triangulate
                std::tie(v0_idx, vt0_idx, vn0_idx) = to_vertex_numbers(sep_s[1]);
                std::tie(v1_idx, vt1_idx, vn1_idx) = to_vertex_numbers(sep_s[2]);
                std::tie(v2_idx, vt2_idx, vn2_idx) = to_vertex_numbers(sep_s[3]);
                std::tie(v3_idx, vt3_idx, vn3_idx) = to_vertex_numbers(sep_s[4]);

                auto n_vertices = vertices.size();
                v0_idx = to_index(v0_idx, n_vertices);
                v1_idx = to_index(v1_idx, n_vertices);
                v2_idx = to_index(v2_idx, n_vertices);
                v3_idx = to_index(v3_idx, n_vertices);
                group.triangles.emplace_back(v0_idx, v1_idx, v2_idx);
                group.triangles.emplace_back(v0_idx, v2_idx, v3_idx);

                if (vt0_idx != 0) {
                    auto n_uv_coordinates = uv_coordinates.size();
                    vt0_idx = to_index(vt0_idx, n_uv_coordinates);
                    vt1_idx = to_index(vt1_idx, n_uv_coordinates);
                    vt2_idx = to_index(vt2_idx, n_uv_coordinates);
                    vt3_idx = to_index(vt3_idx, n_uv_coordinates);
                    group.triangle_uv_coordinates.emplace_back(vt0_idx, vt1_idx, vt2_idx);
                    group.triangle_uv_coordinates.emplace_back(vt0_idx, vt2_idx, vt3_idx);
                }

                if (vn0_idx != 0) {
                    auto n_normals = normals.size();
                    v0_idx = to_index(v0_idx, n_normals);
                    v1_idx = to_index(v1_idx, n_normals);
                    v2_idx = to_index(v2_idx, n_normals);
                    v3_idx = to_index(v3_idx, n_normals);
                    group.vertex_normals.emplace_back(vt0_idx, vt1_idx, vt2_idx);
                    group.vertex_normals.emplace_back(vt0_idx, vt2_idx, vt3_idx);
                }
            }
            prev_keyword = keyword;
        } else if (prev_keyword == "f" && keyword != "f") {
            groups.push_back(group);
            prev_keyword = keyword;
        }
    }
    return true;
}

// std::vector<Surface*> ObjLoader::convert_surface_instances() {}

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
