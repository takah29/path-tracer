#define BOOST_TEST_MODULE utility
#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "../utility.h"

BOOST_AUTO_TEST_SUITE(test_utility_sute)

BOOST_AUTO_TEST_CASE(test_correct_clamp) {
    BOOST_CHECK_EQUAL(clamp(1.1), 1.0);
    BOOST_CHECK_EQUAL(clamp(-0.1), 0.0);
    BOOST_CHECK_EQUAL(clamp(0.5), 0.5);
}

BOOST_AUTO_TEST_CASE(test_correct_to_int) {
    BOOST_CHECK_EQUAL(to_int(0.0), 0);
    BOOST_CHECK_EQUAL(to_int(1.0), 255);

    BOOST_CHECK_EQUAL(to_int(-0.1), 0);
    BOOST_CHECK_EQUAL(to_int(1.1), 255);
}

BOOST_AUTO_TEST_CASE(test_correct_save_ppm) {
    std::string filename = "test_image.ppm";
    int width = 4, height = 3;
    Color c = Color(1.0, 1.0, 1.0);
    std::vector<Color> image(width * height, c);
    save_ppm_file(filename, image, width, height);

    std::ifstream infile(filename);
    std::string line;
    getline(infile, line);
    BOOST_CHECK_EQUAL(line, "P3");
    getline(infile, line);
    BOOST_CHECK_EQUAL(line, "4 3");
    getline(infile, line);
    BOOST_CHECK_EQUAL(line, "255");
    for (int i = 0; i < width * height; i++) {
        getline(infile, line);
        BOOST_CHECK_EQUAL(line, "255 255 255");
    }
}

BOOST_AUTO_TEST_CASE(test_correct_split) {
    std::string s = "abc,def";

    std::vector<std::string> result;
    result = split(s, ',');
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");

    s = "abc,def,";
    result = split(s, ',');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");
    BOOST_CHECK_EQUAL(result[2], "");

    s = "abc,,def";
    result = split(s, ',');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "");
    BOOST_CHECK_EQUAL(result[2], "def");

    s = "abc def";
    result = split(s, ' ');
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");
}

BOOST_AUTO_TEST_SUITE_END()
