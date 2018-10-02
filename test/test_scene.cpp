#define BOOST_TEST_MODULE scene
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <map>
#include "../scene.h"

using namespace std;

BOOST_AUTO_TEST_CASE(test_correct_viewplane_constructor) {
    double plane_width = 640.0;
    int width_res = 640, height_res = 480;
    ViewPlane vp(plane_width, width_res, height_res);

    BOOST_CHECK_EQUAL(vp.plane_height, 480.0);
    BOOST_CHECK_EQUAL(vp.pixel_size, 1.0);
}
