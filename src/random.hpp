#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <random>

struct UniformRealGenerator {
    std::mt19937 mt;
    std::uniform_real_distribution<double> real_generator;

    UniformRealGenerator();
    UniformRealGenerator(const int seed, const double left = 0.0, const double right = 1.0);

    double operator()();
};

#endif
