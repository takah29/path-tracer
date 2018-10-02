#define BOOST_TEST_MODULE tracer
#include <boost/test/included/unit_test.hpp>

#include <random>
#include "../tracer.h"

using namespace std;

BOOST_AUTO_TEST_CASE(test_correct_create_orthonormal_basis) {
    Vec w(1.0, 0.0, 0.0);
    auto[u, v] = create_orthonormal_basis(w);

    double result = dot(w, u);
    BOOST_CHECK_EQUAL(result, 0.0);

    result = dot(w, v);
    BOOST_CHECK_EQUAL(result, 0.0);
}

BOOST_AUTO_TEST_CASE(test_correct_sample_from_hemisphere) {
    Vec w(1.0, 0.0, 0.0);
    auto[u, v] = create_orthonormal_basis(w);

    mt19937 mt;

    Vec result = sample_from_hemisphere(u, v, w, mt, 0.0);

    BOOST_CHECK(result.x >= -1.0);
    BOOST_CHECK(result.x <= 1.0);
    BOOST_CHECK(result.y >= -1.0);
    BOOST_CHECK(result.y <= 1.0);
    BOOST_CHECK(result.z >= -1.0);
    BOOST_CHECK(result.z <= 1.0);
}
