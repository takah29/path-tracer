#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <random>

struct UniformRealGenerator {
    std::mt19937 mt;
    std::uniform_real_distribution<double> real_generator;

    UniformRealGenerator() : mt(0), real_generator(0.0, 1.0) {}
    UniformRealGenerator(const int seed, const double left = 0.0, const double right = 1.0)
        : mt(seed), real_generator(left, right) {}

    double operator()() { return real_generator(mt); }
};

#endif
