#ifndef _BVH_H_
#define _BVH_H_

#include <tuple>
#include <utility>
#include <vector>
#include "object.hpp"

double calc_bbox_area(const BBox &bbox);
void merge_bbox(const BBox &bbox1, const BBox &bbox2, BBox &result);

struct BvhNode {
    BBox bbox;
    std::pair<int, int> child_node_idx;
    std::vector<int> target_indices;

    BvhNode();
};

struct BVH {
    std::vector<BvhNode> bvh_nodes;
    double T_tri, T_aabb;

    BVH();
    BVH(const double &T_tri, const double &T_aabb);

    void enclosing_bbox(const std::vector<std::pair<int, BBox *>> &bboxes, BBox &result);
    void make_leaf(std::vector<std::pair<int, BBox *>> bboxes, BvhNode &bvh_node);
    void axis_decending_sort(std::vector<std::pair<int, BBox *>> &bboxes, const int &axis);
    void construct_bvh(std::vector<std::pair<int, BBox *>> &bboxes, int node_idx);
    void construct(std::vector<BBox *> bboxes);
    void traverse_bvh(const Ray &ray, std::vector<int> &target_indices, int node_idx) const;
    std::vector<int> traverse(const Ray &ray) const;
    void print_bvh_nodes();
};

#endif
