#define BOOST_TEST_MODULE vec
#include <boost/test/included/unit_test.hpp>

#include "../vec.h"

struct Fixture {
    Vec v1, v2;
    double a;

    Fixture() : v1(Vec(1.0, 2.0, 3.0)), v2(Vec(4.0, 5.0, 6.0)), a(1.5) {}
};

BOOST_FIXTURE_TEST_SUITE(test_vec_sute, Fixture)

BOOST_AUTO_TEST_CASE(test_correct_vec_operators) {
    BOOST_CHECK(v1 == v1);

    Vec result;
    result = v1 + v2;
    BOOST_CHECK(result == Vec(5.0, 7.0, 9.0));

    result = v1 - v2;
    BOOST_CHECK(result == Vec(-3.0, -3.0, -3.0));

    result = v1 * a;
    BOOST_CHECK(result == Vec(1.5, 3.0, 4.5));

    result = v1 / a;
    BOOST_CHECK_EQUAL(result.x, 1.0 / 1.5);
    BOOST_CHECK_EQUAL(result.y, 2.0 / 1.5);
    BOOST_CHECK_EQUAL(result.z, 3.0 / 1.5);

    result = Vec(1.0, 1.0, 1.0);
    result += v2;
    BOOST_CHECK(result == Vec(5.0, 6.0, 7.0));

    result = Vec(1.0, 1.0, 1.0);
    result -= v2;
    BOOST_CHECK(result == Vec(-3.0, -4.0, -5.0));

    result = Vec(1.0, 1.0, 1.0);
    result *= 2.0;
    BOOST_CHECK(result == Vec(2.0, 2.0, 2.0));

    result = Vec(1.0, 1.0, 1.0);
    result /= 2.0;
    BOOST_CHECK(result == Vec(0.5, 0.5, 0.5));

    result = Vec(2.0, 0.0, 0.0);
    result.normalize();
    BOOST_CHECK(result == Vec(1.0, 0.0, 0.0));
}

BOOST_AUTO_TEST_CASE(test_correct_multiplyer) { BOOST_CHECK(a * v1 == Vec(1.5, 3.0, 4.5)); }

BOOST_AUTO_TEST_CASE(test_correct_dot) { BOOST_CHECK(dot(v1, v2) == 32.0); }

BOOST_AUTO_TEST_CASE(test_correct_norm) {
    const Vec v(1.0, 0.0, 0.0);
    BOOST_CHECK_EQUAL(norm(v), 1.0);
}

BOOST_AUTO_TEST_CASE(test_correct_normalize) { BOOST_CHECK_EQUAL(norm(normalize(v1)), 1.0); }

BOOST_AUTO_TEST_CASE(test_correct_multiply) {
    BOOST_CHECK(multiply(v1, v2) == Vec(4.0, 10.0, 18.0));
}

BOOST_AUTO_TEST_CASE(test_correct_cross) {
    const Vec a(1.0, 0.0, 0.0), b(0.0, 1.0, 0.0);
    BOOST_CHECK(cross(a, b) == Vec(0.0, 0.0, 1.0));
}

BOOST_AUTO_TEST_SUITE_END()
