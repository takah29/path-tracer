#include "loader.hpp"
#include "wrapper.hpp"

bool load_ply_file(const std::string file_path, Surface* surface) {
    std::ifstream infile(file_path);
    std::string line;

    // ファイルフォーマットの確認
    getline(infile, line);
    if (line != "ply") {
        std::cout << "The file is not ply format." << std::endl;
        return false;
    }

    // ascii形式の確認
    std::string keyword, fileformat;
    int version;
    infile >> keyword >> fileformat >> version;
    if (fileformat != "ascii") {
        std::cout << "The file is not ascii format." << std::endl;
        return false;
    }

    std::vector<std::string> sep_s;
    size_t n_vertices = 0;
    std::vector<std::string> vertex_property;
    size_t n_triangles = 0;
    std::vector<std::string> triangle_property;
    std::string type;
    while (getline(infile, line)) {
        sep_s = split_reg(line, " +");
        keyword = sep_s[0];

        if (keyword == "end_header") break;
        if (keyword == "comment") continue;

        if (keyword == "element") {
            type = sep_s[1];
            if (type == "vertex") {
                n_vertices = stoi(sep_s[2]);
            } else if (type == "face") {
                n_triangles = stoi(sep_s[2]);
            }
        } else if (keyword == "property") {
            if (type == "vertex") {
                vertex_property.push_back(sep_s[2]);
            } else if (type == "face") {
                triangle_property.push_back(sep_s[1]);
            }
        }
    }

    double min_x = INF, min_y = INF, min_z = INF;
    double max_x = -INF, max_y = -INF, max_z = -INF;

    // 頂点の読み込み
    for (size_t i = 0; i < n_vertices; i++) {
        getline(infile, line);
        sep_s = split_reg(line, " +");

        // 頂点座標のみ処理
        double x = stod(sep_s[0]), y = stod(sep_s[1]), z = stod(sep_s[2]);
        surface->vertices.push_back(Vec(x, y, z));

        // バウンディングボックス用に最小値、最大値を算出
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (z < min_z) min_z = z;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
        if (z > max_z) max_z = z;
    }

    // 頂点座標から面を構成
    int n_vert;
    int idx0, idx1, idx2;
    for (size_t i = 0; i < n_triangles; i++) {
        getline(infile, line);
        sep_s = split_reg(line, " +");

        // 頂点数4以上のポリゴンは除外する
        n_vert = stoi(sep_s[0]);
        if (n_vert == 3) {
            idx0 = std::stoi(sep_s[1]);
            idx1 = std::stoi(sep_s[2]);
            idx2 = std::stoi(sep_s[3]);
            surface->triangles.push_back(std::make_tuple(idx0, idx1, idx2));
        }
    }

    infile.close();

    // バウンディングボックス設定
    // Vec corner0(min_x - EPS, min_y - EPS, min_z - EPS);
    // Vec corner1(max_x + EPS, max_y + EPS, max_z + EPS);
    // surface->bbox.set_corner(corner0, corner1);

    // 中心座標の設定
    // surface->center = (corner0 + corner1) / 2.0;

    // 法線の計算
    surface->compute_normals();

    // ポリゴンのバウンディングボックスを作成
    surface->compute_bboxes();

    // 空間データ構造の構築
    surface->construct();

    return surface;
}

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
                                   const int v_offset_idx, const int vt_offset_idx,
                                   const int vn_offset_idx) {
    int idx;
    std::tuple<int, int, int> result = vertex_elements;

    idx = std::get<0>(vertex_elements);
    if (idx > 0) {
        std::get<0>(result) = idx - v_offset_idx - 1;
    } else if (idx < 0) {
        std::get<0>(result) = v_offset_idx + idx;
    }

    idx = std::get<1>(vertex_elements);
    if (idx > 0) {
        std::get<1>(result) = idx - vt_offset_idx - 1;
    } else if (idx < 0) {
        std::get<1>(result) = vt_offset_idx + idx;
    }

    idx = std::get<2>(vertex_elements);
    if (idx > 0) {
        std::get<2>(result) = idx - vn_offset_idx - 1;
    } else if (idx < 0) {
        std::get<2>(result) = vn_offset_idx + idx;
    }

    return result;
}

bool load_objmtl_file(const std::string file_path, std::map<std::string, Material>& materials) {
    print("load mtl file...");
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
        line = strip(line);
        if (line == "" || line[0] == '#') continue;

        sep_s = split_reg(line, " +");
        if (sep_s[0] == "newmtl") {
            material_name = sep_s[1];
            materials[material_name] = Material();
        } else if (sep_s[0] == "Kd") {
            sscanf(line.data(), "Kd %lf %lf %lf", &val0, &val1, &val2);
            texture_ptr = new ConstantTexture(Color(val0, val1, val2));
            materials[material_name].color_ptr = texture_ptr;
        } else if (sep_s[0] == "map_Kd") {
            int pos = file_path.find_last_of("/") + 1;
            std::string image_path = file_path.substr(0, pos) + sep_s[1];
            Image* image_ptr = new Image();
            load_rgb_image_file(image_path, *image_ptr);
            texture_ptr = new ImageTexture(image_ptr, nullptr);
            materials[material_name].color_ptr = texture_ptr;
        }
    }
    return true;
}

bool load_obj_file(const std::string file_path, std::vector<Object*>& objects) {
    print("load obj file...");
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
    int v_offset_idx = 0, vt_offset_idx = 0, vn_offset_idx = 0;
    int v0_idx, v1_idx, v2_idx, v3_idx;
    int vt0_idx, vt1_idx, vt2_idx, vt3_idx;
    int vn0_idx, vn1_idx, vn2_idx, vn3_idx;
    double val0, val1, val2;
    auto materials = new std::map<std::string, Material>;
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
            int pos = file_path.find_last_of("/") + 1;
            std::string mtl_path = file_path.substr(0, pos) + sep_s[1];
            if (!load_objmtl_file(mtl_path, *materials)) {
                printf("mtl file not found.");
                return false;
            }
        } else if (keyword == "o" || keyword == "g") {
            surface = new SmoothSurface();
            surface->set_material(new Material());
            surface->texture_flag = true;
            prev_keyword = keyword;
            count_obj++;
        } else if (keyword == "v") {
            sscanf(line.c_str(), "v  %lf %lf %lf", &val0, &val1, &val2);
            // print("v", val0, val1, val2, "size:", tmp_vertices.size());
            tmp_vertices.push_back(Vec(val0, val1, val2));
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
                std::tie(v0_idx, vt0_idx, vn0_idx) = to_index(
                    get_vertex_elements(sep_s[1]), v_offset_idx, vt_offset_idx, vn_offset_idx);
                std::tie(v1_idx, vt1_idx, vn1_idx) = to_index(
                    get_vertex_elements(sep_s[2]), v_offset_idx, vt_offset_idx, vn_offset_idx);
                std::tie(v2_idx, vt2_idx, vn2_idx) = to_index(
                    get_vertex_elements(sep_s[3]), v_offset_idx, vt_offset_idx, vn_offset_idx);

                surface->triangles.push_back(std::make_tuple(v0_idx, v1_idx, v2_idx));

                if (vt0_idx > 0) {
                    surface->triangle_uv_coordinates.push_back(
                        std::make_tuple(vt0_idx, vt1_idx, vt2_idx));
                }
            } else if (sep_s.size() == 5) {  // triangulate
                std::tie(v0_idx, vt0_idx, vn0_idx) = to_index(
                    get_vertex_elements(sep_s[1]), v_offset_idx, vt_offset_idx, vn_offset_idx);
                std::tie(v1_idx, vt1_idx, vn1_idx) = to_index(
                    get_vertex_elements(sep_s[2]), v_offset_idx, vt_offset_idx, vn_offset_idx);
                std::tie(v2_idx, vt2_idx, vn2_idx) = to_index(
                    get_vertex_elements(sep_s[3]), v_offset_idx, vt_offset_idx, vn_offset_idx);
                std::tie(v3_idx, vt3_idx, vn3_idx) = to_index(
                    get_vertex_elements(sep_s[4]), v_offset_idx, vt_offset_idx, vn_offset_idx);

                surface->triangles.push_back(std::make_tuple(v0_idx, v1_idx, v2_idx));
                surface->triangles.push_back(std::make_tuple(v0_idx, v2_idx, v3_idx));

                if (vt0_idx > 0) {
                    surface->triangle_uv_coordinates.push_back(
                        std::make_tuple(vt0_idx, vt1_idx, vt2_idx));
                    surface->triangle_uv_coordinates.push_back(
                        std::make_tuple(vt0_idx, vt2_idx, vt3_idx));
                }
                //                print(vt0_idx, vt1_idx, vt2_idx, tmp_uv_coordinates.size());
            }
            prev_keyword = keyword;
        } else if (prev_keyword == "f" && keyword != "f") {
            surface->vertices = tmp_vertices;
            surface->uv_coordinates = tmp_uv_coordinates;
            surface->compute_normals();
            surface->compute_bboxes();
            surface->construct();

            objects.push_back(surface);
            prev_keyword = keyword;
            v_offset_idx += tmp_vertices.size();
            vt_offset_idx += tmp_uv_coordinates.size();
            vn_offset_idx += tmp_normals.size();
            tmp_vertices.clear();
            tmp_uv_coordinates.clear();
            tmp_normals.clear();
        }
    }
    return true;
}
