#ifndef _BVH_H_
#define _BVH_H_

#include <tuple>
#include <utility>
#include <vector>
#include "object.h"

double calc_bbox_area(const BBox &bbox) {
    double len_x = bbox.corner1.x - bbox.corner0.x;
    double len_y = bbox.corner1.y - bbox.corner0.y;
    double len_z = bbox.corner1.z - bbox.corner0.z;
    return 2. * (len_x * len_y + len_x * len_z + len_y * len_z);
}

void merge_bbox(const BBox &bbox1, const BBox &bbox2, BBox &result) {
    result.set_corner(min(bbox1.corner0, bbox2.corner0), max(bbox1.corner1, bbox2.corner1));
}

struct BvhNode {
    BBox bbox;
    std::pair<int, int> child_node_idx;
    std::vector<int> target_indices;

    BvhNode() : child_node_idx(-1, -1), target_indices(0) { this->bbox.empty(); }
};

struct BVH {
    std::vector<BvhNode> bvh_nodes;
    double T_tri, T_aabb;

    BVH() : bvh_nodes(1), T_tri(1.0), T_aabb(1.0) {}
    BVH(const double &T_tri, const double &T_aabb) : bvh_nodes(1), T_tri(T_tri), T_aabb(T_aabb) {}

    void enclosing_bbox(const std::vector<std::pair<int, BBox *>> &bboxes, BBox &result) {
        for (auto idx_bbox : bboxes) {
            merge_bbox(*idx_bbox.second, result, result);
        }
    }

    void make_leaf(std::vector<std::pair<int, BBox *>> bboxes, BvhNode &bvh_node) {
        bvh_node.child_node_idx.first = -1;
        bvh_node.child_node_idx.second = -1;
        for (const auto &x : bboxes) {
            bvh_node.target_indices.push_back(x.first);
        }
    }

    void axis_decending_sort(std::vector<std::pair<int, BBox *>> &bboxes, const int &axis) {
        sort(bboxes.begin(), bboxes.end(),
             [axis](const std::pair<int, BBox *> &a, const std::pair<int, BBox *> &b) {
                 switch (axis) {
                     case 0:
                         return a.second->center.x > b.second->center.x;
                     case 1:
                         return a.second->center.y > b.second->center.y;
                     case 2:
                         return a.second->center.z > b.second->center.z;
                 }
                 return a.second->center.x > b.second->center.x;
             });
    }

    void construct_bvh_internal(std::vector<std::pair<int, BBox *>> &bboxes, int node_idx) {
        // 全体を囲うバウンディングボックスを求める
        enclosing_bbox(bboxes, bvh_nodes[node_idx].bbox);

        // 葉ノードの場合の暫定コスト
        double best_cost = T_tri * bboxes.size();

        int best_axis = -1;
        int best_split_idx = -1;

        double root_bbox_area = calc_bbox_area(bvh_nodes[node_idx].bbox);

        for (int axis = 0; axis < 3; axis++) {
            axis_decending_sort(bboxes, axis);
            std::vector<std::pair<int, BBox *>> s1, s2(bboxes);

            std::vector<double> s1_bbox_areas(bboxes.size() + 1, INF);
            std::vector<double> s2_bbox_areas(bboxes.size() + 1, INF);

            BBox s1_bbox;
            s1_bbox.empty();
            // bboxのサイズが増加する計算のほうが効率的なのでs1_bboxとs2_bboxの面積計算を分けている
            // s1_bboxが増加する場合の面積計算
            for (size_t i = 0; i <= bboxes.size(); i++) {
                s1_bbox_areas[i] = std::abs(calc_bbox_area(s1_bbox));

                if (s2.size() > 0) {
                    std::pair<int, BBox *> idx_bbox = s2.back();
                    s1.push_back(idx_bbox);
                    s2.pop_back();
                    merge_bbox(s1_bbox, *idx_bbox.second, s1_bbox);
                }
            }

            BBox s2_bbox;
            s2_bbox.empty();
            // s2_bboxが増加する場合の面積計算
            for (int i = bboxes.size(); i >= 0; i--) {
                s2_bbox_areas[i] = std::abs(calc_bbox_area(s2_bbox));
                if (s1.size() > 0 && s2.size() > 0) {
                    double cost =
                        2 * T_aabb + (s1_bbox_areas[i] * s1.size() + s2_bbox_areas[i] * s2.size()) *
                                         T_tri / root_bbox_area;
                    if (cost < best_cost) {
                        best_cost = cost;
                        best_axis = axis;
                        best_split_idx = i;
                    }
                }

                if (s1.size() > 0) {
                    std::pair<int, BBox *> idx_bbox = s1.back();
                    s2.push_back(idx_bbox);
                    s1.pop_back();
                    merge_bbox(s2_bbox, *idx_bbox.second, s2_bbox);
                }
            }
        }

        if (best_axis == -1) {
            make_leaf(bboxes, bvh_nodes[node_idx]);
        } else {
            // コスト最小の軸でソート
            axis_decending_sort(bboxes, best_axis);

            // 子ノード作成
            bvh_nodes[node_idx].child_node_idx.first = bvh_nodes.size();
            bvh_nodes.push_back(BvhNode());
            bvh_nodes[node_idx].child_node_idx.second = bvh_nodes.size();
            bvh_nodes.push_back(BvhNode());

            // vectorの分割
            std::vector<std::pair<int, BBox *>> left(bboxes.begin(),
                                                     bboxes.begin() + best_split_idx);
            std::vector<std::pair<int, BBox *>> right(bboxes.begin() + best_split_idx,
                                                      bboxes.end());

            // 再帰呼出し
            construct_bvh_internal(left, bvh_nodes[node_idx].child_node_idx.first);
            construct_bvh_internal(right, bvh_nodes[node_idx].child_node_idx.second);
        }
    }

    void construct(std::vector<BBox *> bboxes) {
        std::vector<std::pair<int, BBox *>> id_bboxes(bboxes.size());
        for (size_t i = 0; i < bboxes.size(); i++) {
            id_bboxes[i] = std::make_pair(i, bboxes[i]);
        }

        construct_bvh_internal(id_bboxes, 0);
    }

    void traverse_bvh(const Ray &ray, std::vector<int> &target_indices, int node_idx) const {
        if (bvh_nodes[node_idx].bbox.hit(ray)) {
            if (bvh_nodes[node_idx].child_node_idx.first != -1) {
                traverse_bvh(ray, target_indices, bvh_nodes[node_idx].child_node_idx.first);
                traverse_bvh(ray, target_indices, bvh_nodes[node_idx].child_node_idx.second);
            } else {
                target_indices.insert(target_indices.end(), bvh_nodes[node_idx].target_indices.begin(),
                                      bvh_nodes[node_idx].target_indices.end());
            }
        }
    }

    std::vector<int> traverse(const Ray &ray) const {
        std::vector<int> target_indices(0);
        traverse_bvh(ray, target_indices, 0);
        return target_indices;
    }

    void print_bvh_nodes() {
        for (auto a : bvh_nodes) {
            printf("=================================\n\n");
            if (a.child_node_idx.first != -1) {
                printf("-----Inter Node-----\n");
                printf("bbox: corner0=(%f, %f, %f), corner1=(%f, %f, %f)\n", a.bbox.corner0.x,
                       a.bbox.corner0.y, a.bbox.corner0.z, a.bbox.corner1.x, a.bbox.corner1.y,
                       a.bbox.corner1.z);

                printf("child node idx: left=%d, right=%d\n", a.child_node_idx.first,
                       a.child_node_idx.second);

                printf("target indices: ");
                for (auto idx : a.target_indices) printf("%d", idx);
                printf("\n\n");
            } else {
                printf("-----Leaf Node-----\n");
                printf("bbox: corner0=(%f, %f, %f), corner1=(%f, %f, %f)\n", a.bbox.corner0.x,
                       a.bbox.corner0.y, a.bbox.corner0.z, a.bbox.corner1.x, a.bbox.corner1.y,
                       a.bbox.corner1.z);

                printf("child node idx: left=%d, right=%d\n", a.child_node_idx.first,
                       a.child_node_idx.second);

                printf("target indices: ");
                for (auto idx : a.target_indices) printf("%d, ", idx);
                printf("\n\n");
            }
        }
        printf("=================================\n\n");
    }
};

#endif
