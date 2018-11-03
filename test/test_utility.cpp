#define BOOST_TEST_MODULE utility
#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/utility.hpp"

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

BOOST_AUTO_TEST_CASE(test_correct_linspace) {
    double start = 123, end = 456;
    int n = 50;

    auto result = linspace(start, end, n);
    BOOST_CHECK_EQUAL(result.size(), n);
    BOOST_CHECK_EQUAL(result[0], start);
    BOOST_CHECK_EQUAL(result[n - 1], end);

    start = 5;
    end = 10;
    n = 5;
    result = linspace(start, end, n, false);
    BOOST_CHECK_EQUAL(result.size(), n);
    BOOST_CHECK_EQUAL(result[0], start);
    BOOST_CHECK_EQUAL(result[n - 1], 9.0);
}

BOOST_AUTO_TEST_CASE(test_correct_save_and_load_ppm_file) {
    // save ppm file
    std::string filename = "test_image.ppm";
    int width_res = 4, height_res = 3;
    Color c = Color(1.0, 1.0, 1.0);
    Image image(width_res, height_res);

    for (int i = 0; i < image.height_res; i++) {
        for (int j = 0; j < image.width_res; j++) {
            image.set_color(c, i, j);
        }
    }
    save_ppm_file(filename, image);

    // load ppm file
    Image load_image = load_ascii_ppm_file(filename);

    BOOST_CHECK_EQUAL(load_image.width_res, width_res);
    BOOST_CHECK_EQUAL(load_image.height_res, height_res);
    for (int i = 0; i < load_image.height_res; i++) {
        for (int j = 0; j < load_image.width_res; j++) {
            BOOST_CHECK(load_image.get_color(i, j) == c);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_correct_strip) {
    std::string s(" abc,def");
    std::string result = strip(s);
    BOOST_CHECK_EQUAL(result, "abc,def");

    s = "abc,def ";
    result = strip(s);
    BOOST_CHECK_EQUAL(result, "abc,def");

    s = " abc,def ";
    result = strip(s);
    BOOST_CHECK_EQUAL(result, "abc,def");

    s = "   abc,def   ";
    result = strip(s);
    BOOST_CHECK_EQUAL(result, "abc,def");

    s = ",abc,def,";
    result = strip(s, ",");
    BOOST_CHECK_EQUAL(result, "abc,def");
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

    s = "abc  def";
    result = split(s, ' ');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "");
    BOOST_CHECK_EQUAL(result[2], "def");
}

BOOST_AUTO_TEST_CASE(test_correct_split_reg) {
    std::string s = "abc,def";

    std::vector<std::string> result;
    result = split_reg(s, ",");
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");

    s = "abc,def,";
    result = split_reg(s, ",");
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");

    s = "abc,,def";
    result = split_reg(s, ",");
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "");
    BOOST_CHECK_EQUAL(result[2], "def");

    s = "abc def";
    result = split_reg(s, " ");
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");

    s = "abc  def";
    result = split_reg(s, " +");
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");

    s = "abc def ";
    result = split_reg(s, " +");
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "abc");
    BOOST_CHECK_EQUAL(result[1], "def");
}

BOOST_AUTO_TEST_SUITE_END()
