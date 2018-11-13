#include "random.hpp"

UniformRealGenerator::UniformRealGenerator() : mt(0), real_generator(0.0, 1.0) {}
UniformRealGenerator::UniformRealGenerator(const int seed, const double left, const double right)
    : mt(seed), real_generator(left, right) {}

double UniformRealGenerator::operator()() { return real_generator(mt); }
