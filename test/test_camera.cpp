#define BOOST_TEST_MODULE camera
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <map>
#include "../src/camera.hpp"

using namespace std;

BOOST_AUTO_TEST_SUITE(test_camera_sute)

BOOST_AUTO_TEST_SUITE(test_pinhole_sute)

BOOST_AUTO_TEST_CASE(test_correct_ray_direction) {
    Vec eye(0.0, 0.0, 0.0), lookat(1.0, 0.0, 0.0);
    float d = 0.5;

    Pinhole camera(eye, lookat, d);
    Vec dir(camera.ray_direction(Point2D(0.0, 0.0)));
    BOOST_CHECK(dir == Vec(1.0, 0.0, 0.0));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
