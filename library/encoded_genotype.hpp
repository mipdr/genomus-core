#ifndef __GENOMUS_CORE_ENCODED_GENOTYPE__
#define __GENOMUS_CORE_ENCODED_GENOTYPE__ 

#include <vector>
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"

#define GERMINAL_VECTOR_MAX_LENGTH 64
#define MAX_GENOTYPE_VECTOR_SIZE 128
#define MAX_GENOTYPE_DEPTH 10

// Generate a random vector of size n
std::vector<double> randomVector(int n);

// Generate a random vector of random size until GERMINAL_VECTOR_MAX_LENGTH
std::vector<double> newGerminalVector();

// State machine to solve non-deterministic generative gramatic
// for genotype retrotranscription.
struct VectorNormalizationState {
    // size_t position;
    EncodedPhenotypeType output_type;
    size_t current_depth;
    // VectorNormalizationState getCopy() const {
    //     return {
    //         .position = this -> position,
    //         .output_type = this -> output_type,
    //         .current_depth = this -> current_depth,
    //     };
    // };
};

void normalizeVector(std::vector<double>& input, std::vector<double>& output);

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