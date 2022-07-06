#ifndef __GENOMUS_CORE_ENCODED_GENOTYPE__
#define __GENOMUS_CORE_ENCODED_GENOTYPE__ 

#include <vector>
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"

#define GERMINAL_VECTOR_MAX_LENGTH 256
#define MAX_GENOTYPE_VECTOR_SIZE 10000
#define MAX_GENOTYPE_DEPTH 256
#define MAX_LIST_SIZE 128
#define LIST_EXTENSION_THRESHOLD std::min(0.5, 1.0 / (double)MAX_LIST_SIZE)

// Generate a random vector of size n
std::vector<double> randomVector(int n);

// Generate a random vector of random size until GERMINAL_VECTOR_MAX_LENGTH
std::vector<double> newGerminalVector();

// State machine to solve non-deterministic generative gramatic
// for genotype retrotranscription.
struct VectorNormalizationState {
    EncodedPhenotypeType output_type;
    size_t current_depth;
};

void normalizeVector(const std::vector<double>& input, std::vector<double>& output);
std::string toExpression(const std::vector<double>& input);
// dec_gen_t toDecodedGenotype(const std::vector<double>& input);

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