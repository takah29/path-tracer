#include "loader.hpp"

// 4 corresponding formats
// format1: "v"       -> tuple(v, 0, 0)
// format2: "v/vt"    -> tuple(v, vt, 0)
// format3: "v/vt/vn" -> tuple(v, vt, vn)
// format4: "v//vn"   -> tuple(v, 0, vn)
std::tuple<int, int, int> get_vertex_elements(std::string s) {
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

std::tuple<int, int, int> to_index(const std::tuple<int, int, int>& vertex_elements,
                                   const int offset_idx) {
    int v_idx;
    std::tuple<int, int, int> positive_elements = vertex_elements;

    v_idx = std::get<0>(vertex_elements);
    if (v_idx > 0) {
        std::get<0>(positive_elements) = v_idx - offset_idx - 1;
    } else if (v_idx < 0) {
        std::get<0>(positive_elements) = offset_idx + v_idx;
    }

    v_idx = std::get<1>(vertex_elements);
    if (v_idx > 0) {
        std::get<1>(positive_elements) = v_idx - offset_idx - 1;
    } else if (v_idx < 0) {
        std::get<1>(positive_elements) = offset_idx + v_idx;
    }

    v_idx = std::get<2>(vertex_elements);
    if (v_idx > 0) {
        std::get<2>(positive_elements) = v_idx - offset_idx - 1;
    } else if (v_idx < 0) {
        std::get<2>(positive_elements) = offset_idx + v_idx;
    }

    return positive_elements;
}

bool load_objmtl_file(const std::string file_path, std::map<std::string, Material>& materials) {
    std::ifstream infile(file_path);
    std::string line;

    std::string keyword;
    std::vector<std::string> sep_s;
    std::string material_name;
    Material material;
    double val0, val1, val2;
    Texture* texture_ptr;
    while (!infile.eof()) {
        getline(infile, line);
        if (line == "" || line[0] == '#') continue;

        sep_s = split_reg(line, " +");
        if (sep_s[0] == "newmtl") {
            material_name = sep_s[1];
            materials[material_name] = Material();
        } else if (sep_s[0] == "\tKd") {
            sscanf(line.data(), "\tKd %lf %lf %lf", &val0, &val1, &val2);
            // texture_ptr = new ConstantTexture(Color(val0, val1, val2));
            texture_ptr = new ConstantTexture(RED);
            materials[material_name].color_ptr = texture_ptr;
        }
    }
    return true;
}

bool load_obj_file(const std::string file_path, std::vector<Object*>& objects) {
    std::ifstream infile(file_path);
    std::string line;
    std::vector<std::string> str_vec;

    std::string keyword;
    std::vector<std::string> sep_s;
    std::vector<Vec> tmp_vertices;
    std::vector<Vec> tmp_normals;
    std::vector<std::pair<double, double>> tmp_uv_coordinates;
    SmoothSurface* surface = nullptr;
    std::string prev_keyword;
    int vertex_count = 0, offset_idx = 0;
    int v0_idx, v1_idx, v2_idx, v3_idx;
    int vt0_idx, vt1_idx, vt2_idx, vt3_idx;
    int vn0_idx, vn1_idx, vn2_idx, vn3_idx;
    double val0, val1, val2;
    auto materials = new std::map<std::string, Material>;
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
            int pos = file_path.find_last_of("/") + 1;
            std::string mtl_path = file_path.substr(0, pos) + sep_s[1];
            if (!load_objmtl_file(mtl_path, *materials)) {
                printf("mtl file not found.");
                return false;
            }
        } else if (keyword == "o" || keyword == "g") {
            surface = new SmoothSurface();
            surface->set_material(new Material());
            prev_keyword = keyword;
        } else if (keyword == "v") {
            sscanf(line.c_str(), "v  %lf %lf %lf", &val0, &val1, &val2);
            // print("v", val0, val1, val2, "size:", tmp_vertices.size());
            tmp_vertices.push_back(Vec(val0, val1, val2));
            vertex_count += 1;
            prev_keyword = keyword;
        } else if (keyword == "vt") {
            sscanf(line.c_str(), "vt %lf %lf %lf", &val0, &val1, &val2);
            // print("vt:", val0, val1, val2, "size:", tmp_uv_coordinates.size());
            tmp_uv_coordinates.push_back(std::make_pair(val0, val1));
            prev_keyword = keyword;
        } else if (keyword == "vn") {
            sscanf(line.c_str(), "vn %lf %lf %lf", &val0, &val1, &val2);
            // print("vn:", val0, val1, val2);
            tmp_normals.push_back(Vec(val0, val1, val2));
            prev_keyword = keyword;
        } else if (keyword == "usemtl") {
            Material* mtl = &materials->at(sep_s[1]);
            surface->set_material(mtl);
        } else if (keyword == "f") {
            if (sep_s.size() == 4) {
                std::tie(v0_idx, vt0_idx, vn0_idx) =
                    to_index(get_vertex_elements(sep_s[1]), offset_idx);
                std::tie(v1_idx, vt1_idx, vn1_idx) =
                    to_index(get_vertex_elements(sep_s[2]), offset_idx);
                std::tie(v2_idx, vt2_idx, vn2_idx) =
                    to_index(get_vertex_elements(sep_s[3]), offset_idx);

                surface->triangles.push_back(std::make_tuple(v0_idx, v1_idx, v2_idx));

                if (vt0_idx > 0) {
                    surface->triangle_uv_coordinates.push_back(
                        std::make_tuple(vt0_idx, vt1_idx, vt2_idx));
                }
            } else if (sep_s.size() == 5) {  // triangulate
                std::tie(v0_idx, vt0_idx, vn0_idx) =
                    to_index(get_vertex_elements(sep_s[1]), offset_idx);
                std::tie(v1_idx, vt1_idx, vn1_idx) =
                    to_index(get_vertex_elements(sep_s[2]), offset_idx);
                std::tie(v2_idx, vt2_idx, vn2_idx) =
                    to_index(get_vertex_elements(sep_s[3]), offset_idx);
                std::tie(v3_idx, vt3_idx, vn3_idx) =
                    to_index(get_vertex_elements(sep_s[4]), offset_idx);

                surface->triangles.push_back(std::make_tuple(v0_idx, v1_idx, v2_idx));
                surface->triangles.push_back(std::make_tuple(v0_idx, v2_idx, v3_idx));

                if (vt0_idx > 0) {
                    surface->triangle_uv_coordinates.push_back(
                        std::make_tuple(vt0_idx, vt1_idx, vt2_idx));
                    surface->triangle_uv_coordinates.push_back(
                        std::make_tuple(vt0_idx, vt2_idx, vt3_idx));
                }
            }
            prev_keyword = keyword;
        } else if (prev_keyword == "f" && keyword != "f") {
            surface->vertices = tmp_vertices;
            surface->uv_coordinates = tmp_uv_coordinates;
            surface->has_uv = false;
            surface->compute_normals();
            surface->compute_bboxes();
            surface->construct();

            objects.push_back(surface);
            prev_keyword = keyword;
            offset_idx += tmp_vertices.size();
            tmp_vertices.clear();
            tmp_uv_coordinates.clear();
            tmp_normals.clear();
        }
    }
    return true;
}
