#include "encoded_genotype.hpp"

#include <random>
#include <stdexcept>

static const unsigned int MAX_RANDOM_VECTOR_LENGTH = 2000;

std::vector<float> randomVector(int n) {
    std::vector<float> v;
    auto generator = new std::random_device;

    if (n > MAX_RANDOM_VECTOR_LENGTH) {
        throw std::runtime_error("N is too large");
    }

    for (int i; i < n; ++i) {
        v.push_back((*generator)());
    }
    return v;
}