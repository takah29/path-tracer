#define BOOST_TEST_MODULE random
#include <boost/test/included/unit_test.hpp>

#include "../src/random.hpp"

BOOST_AUTO_TEST_SUITE(test_uniform_real_generator_sute)
BOOST_AUTO_TEST_CASE(test_correct_rand) {
    UniformRealGenerator rnd;

    int N = 1000;
    double result;

    for (int i = 0; i < N; i++) {
        result = rnd();
        BOOST_CHECK_GE(result, 0.0);
        BOOST_CHECK_LE(result, 1.0);
    }

    rnd = UniformRealGenerator(0, 100.0, 200.0);
    for (int i = 0; i < N; i++) {
        result = rnd();
        BOOST_CHECK_GE(result, 100.0);
        BOOST_CHECK_LE(result, 200.0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
