#ifndef __GENOMUS_CORE_ENCODED_GENOTYPE__
#define __GENOMUS_CORE_ENCODED_GENOTYPE__ 

#include <vector>
#include "decoded_genotype.hpp"

#define GERMINAL_VECTOR_MAX_LENGTH 64

std::vector<double> randomVector(int n);
std::vector<double> newGerminalVector();

class EncodedGenotype {
    private:
        std::vector<double> _normalized_vector;
    public:
        EncodedGenotype(const std::vector<double>& germinalVector);
        std::vector<double> getNormalizedVector() const;
        dec_gen_t toDecodedGenotype() const;
        std::string toString() const;
};

using enc_gen_t = std::vector<double>;

#endif