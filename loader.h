#ifndef _LOADER_H_
#define _LOADER_H_

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "surface.h"
#include "utility.h"

template <typename S>
bool from_ply_file(const std::string file_path, S* surface) {
    std::ifstream infile(file_path);
    std::string line;

    // ファイルフォーマットの確認
    getline(infile, line);
    if (line != "ply") {
        std::cout << "The file is not ply file." << std::endl;
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
    size_t n_vertices;
    std::vector<std::string> vertex_property;
    size_t n_triangles;
    std::vector<std::string> triangle_property;
    std::string type;
    while (getline(infile, line)) {
        sep_s = split(line, ' ');
        keyword = sep_s[0];

        if (keyword == "end_header") break;

        if (keyword == "comment") {
            continue;
        } else if (keyword == "element") {
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
        sep_s = split(line, ' ');

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
        sep_s = split(line, ' ');

        // 頂点数4以上のポリゴンは除外する
        n_vert = stoi(sep_s[0]);
        if (n_vert == 3) {
            idx0 = stoi(sep_s[1]);
            idx1 = stoi(sep_s[2]);
            idx2 = stoi(sep_s[3]);
            surface->triangles.push_back(std::make_tuple(idx0, idx1, idx2));
        }
    }

    infile.close();

    // バウンディングボックス設定
    Vec corner0(min_x - EPS, min_y - EPS, min_z - EPS);
    Vec corner1(max_x + EPS, max_y + EPS, max_z + EPS);
    surface->bbox.set_corner(corner0, corner1);

    // 中心座標の設定
    surface->center = (corner0 + corner1) / 2.0;

    // 法線の計算
    surface->compute_normals();

    // ポリゴンのバウンディングボックスを作成
    surface->compute_bboxes();

    // 空間データ構造の構築
    surface->construct();

    return surface;
}

#endif
