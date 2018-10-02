#define BOOST_TEST_MODULE bvh
#include <boost/test/included/unit_test.hpp>

#include <utility>
#include <vector>
#include "../bvh.h"

BOOST_AUTO_TEST_CASE(test_correct_calc_bbox_area) {
    Vec corner0(-1.0, 0.0, -1.0), corner1(1.0, 2.0, 1.0);
    BBox bbox(corner0, corner1);

    double result = calc_bbox_area(bbox);
    BOOST_CHECK_EQUAL(result, 24.0);
}

BOOST_AUTO_TEST_CASE(test_correct_merge_box) {
    Vec corner0(-1.0, 0.0, -1.0), corner1(1.0, 2.0, 1.0);
    BBox bbox1(corner0, corner1);
    BBox bbox2(corner0 + 2.0, corner1 + 2.0);
    BBox result;

    merge_bbox(bbox1, bbox2, result);
    BOOST_CHECK(result.corner0 == Vec(-1.0, 0.0, -1.0));
    BOOST_CHECK(result.corner1 == Vec(3.0, 4.0, 3.0));
    BOOST_CHECK(result.center == Vec(1.0, 2.0, 1.0));
}

struct Fixture {};

BOOST_FIXTURE_TEST_SUITE(test_bvh_sute, Fixture)
BOOST_AUTO_TEST_CASE(test_correct_bvh_enclosing_bbox) {
    BBox bbox1(Vec(-1.0, 0.0, -1.0), Vec(0.0, 1.0, 0.0));
    BBox bbox2(Vec(0.0, 1.0, 0.0), Vec(1.0, 2.0, 1.0));
    BBox bbox3(Vec(1.0, 2.0, 1.0), Vec(2.0, 3.0, 2.0));
    std::vector<std::pair<int, BBox *>> bboxes;
    bboxes.push_back(std::make_pair(0, &bbox1));
    bboxes.push_back(std::make_pair(1, &bbox2));
    bboxes.push_back(std::make_pair(2, &bbox3));

    BVH bvh;
    BBox result;
    result.empty();
    bvh.enclosing_bbox(bboxes, result);

    BOOST_CHECK(result.corner0 == Vec(-1.0, 0.0, -1.0));
    BOOST_CHECK(result.corner1 == Vec(2.0, 3.0, 2.0));
    BOOST_CHECK(result.center == Vec(0.5, 1.5, 0.5));
}

BOOST_AUTO_TEST_CASE(test_correct_bvh_make_leaf) {
    BBox bbox1(Vec(-1.0, 0.0, -1.0), Vec(0.0, 1.0, 0.0));
    BBox bbox2(Vec(0.0, 1.0, 0.0), Vec(1.0, 2.0, 1.0));
    BBox bbox3(Vec(1.0, 2.0, 1.0), Vec(2.0, 3.0, 2.0));
    std::vector<std::pair<int, BBox *>> bboxes;
    bboxes.push_back(std::make_pair(1000, &bbox1));
    bboxes.push_back(std::make_pair(20, &bbox2));
    bboxes.push_back(std::make_pair(500, &bbox3));

    BVH bvh;
    BvhNode result;
    bvh.make_leaf(bboxes, result);

    BOOST_CHECK_EQUAL(result.child_node_idx.first, -1);
    BOOST_CHECK_EQUAL(result.child_node_idx.second, -1);
    BOOST_CHECK_EQUAL(result.target_indices[0], 1000);
    BOOST_CHECK_EQUAL(result.target_indices[1], 20);
    BOOST_CHECK_EQUAL(result.target_indices[2], 500);
}

BOOST_AUTO_TEST_CASE(test_correct_bvh_axis_decending_sort) {
    BBox bbox1(Vec(-1.0, 0.0, -1.0), Vec(0.0, 1.0, 0.0));
    BBox bbox2(Vec(1.0, 2.0, 1.0), Vec(2.0, 3.0, 2.0));
    BBox bbox3(Vec(0.0, 1.0, 0.0), Vec(1.0, 2.0, 1.0));

    std::vector<std::pair<int, BBox *>> bboxes;
    bboxes.push_back(std::make_pair(0, &bbox1));
    bboxes.push_back(std::make_pair(1, &bbox2));
    bboxes.push_back(std::make_pair(2, &bbox3));

    BVH bvh;
    bvh.axis_decending_sort(bboxes, 0);
    BOOST_CHECK_EQUAL(bboxes[0].first, 1);
    BOOST_CHECK_EQUAL(bboxes[1].first, 2);
    BOOST_CHECK_EQUAL(bboxes[2].first, 0);

    bvh.axis_decending_sort(bboxes, 1);
    BOOST_CHECK_EQUAL(bboxes[0].first, 1);
    BOOST_CHECK_EQUAL(bboxes[1].first, 2);
    BOOST_CHECK_EQUAL(bboxes[2].first, 0);

    bvh.axis_decending_sort(bboxes, 2);
    BOOST_CHECK_EQUAL(bboxes[0].first, 1);
    BOOST_CHECK_EQUAL(bboxes[1].first, 2);
    BOOST_CHECK_EQUAL(bboxes[2].first, 0);

}

BOOST_AUTO_TEST_SUITE_END()
