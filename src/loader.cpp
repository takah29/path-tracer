#include "loader.hpp"

bool load_obj_file(const std::string file_path, std::vector<Object*>& objects) {
    std::ifstream infile(file_path);
    std::string line;

    std::string keyword;
    std::vector<std::string> sep_s;
    std::vector<Vec> tmp_vertices;
    std::vector<Vec> tmp_normals;
    std::vector<std::pair<double, double>> tmp_uv_coordinates;
    SmoothSurface* surface = nullptr;
    std::string prev_keyword;
    int vertex_count = 0, subidx = 0;
    int count_line = 0;
    while (!infile.eof()) {
        getline(infile, line);
        count_line++;
        sep_s = split_reg(line, " +");

        keyword = sep_s[0];

        // TODO: マテリアル取得の実装
        if (keyword == "mtllib") continue;
        if (keyword == "usemtl") continue;

        if (keyword == "o" || keyword == "g") {
            surface = new SmoothSurface();
            prev_keyword = keyword;
        } else if (keyword == "v") {
            tmp_vertices.push_back(
                Vec(std::stod(sep_s[1]), std::stod(sep_s[2]), std::stod(sep_s[3])));
            vertex_count += 1;
            prev_keyword = keyword;
        } else if (keyword == "vt") {
            tmp_uv_coordinates.push_back(std::make_pair(std::stod(sep_s[1]), std::stod(sep_s[2])));
            prev_keyword = keyword;
        } else if (keyword == "vn") {
            tmp_normals.push_back(
                Vec(std::stod(sep_s[1]), std::stod(sep_s[2]), std::stod(sep_s[3])));
            prev_keyword = keyword;
        } else if (keyword == "f") {
            if (sep_s.size() == 4) {
                surface->triangles.push_back(
                    std::make_tuple(std::stoi(split_reg(sep_s[1], "/")[0]) - subidx - 1,
                                    std::stoi(split_reg(sep_s[2], "/")[0]) - subidx - 1,
                                    std::stoi(split_reg(sep_s[3], "/")[0]) - subidx - 1));
            } else if (sep_s.size() == 5) {  // triangulate
                surface->triangles.push_back(
                    std::make_tuple(std::stoi(split_reg(sep_s[1], "/")[0]) - subidx - 1,
                                    std::stoi(split_reg(sep_s[2], "/")[0]) - subidx - 1,
                                    std::stoi(split_reg(sep_s[3], "/")[0]) - subidx - 1));
                surface->triangles.push_back(
                    std::make_tuple(std::stoi(split_reg(sep_s[1], "/")[0]) - subidx - 1,
                                    std::stoi(split_reg(sep_s[3], "/")[0]) - subidx - 1,
                                    std::stoi(split_reg(sep_s[4], "/")[0]) - subidx - 1));
            }
            prev_keyword = keyword;
        } else if (prev_keyword == "f" && keyword != "f") {
            surface->vertices = tmp_vertices;
            // print("vs, fs, subidx: ", surface->vertices.size(), surface->triangles.size(), subidx);
            surface->compute_normals();
            surface->compute_bboxes();
            surface->construct();
            objects.push_back(surface);
            subidx += tmp_vertices.size();
            tmp_vertices.clear();
            tmp_uv_coordinates.clear();
            prev_keyword = keyword;
        }
    }
    return true;
}
