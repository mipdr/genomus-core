#include "encoded_genotype.hpp"
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "errorCodes.hpp"
#include "utils.hpp"

#include <random>
#include <stack>
#include <stdexcept>

static const unsigned int MAX_RANDOM_VECTOR_LENGTH = 2000;

std::vector<double> randomVector(int n) {
    std::vector<double> v;
    auto generator = new std::random_device;

    if (n > MAX_RANDOM_VECTOR_LENGTH) {
        throw std::runtime_error("N is too large");
    }

    for (int i; i < n; ++i) {
        v.push_back((*generator)());
    }
    return v;
}

std::vector<double> getGerminalVector() {
    auto generator = new std::random_device;
    size_t size = (*generator)() * MAX_RANDOM_VECTOR_LENGTH;
    return randomVector(size);
}

// See retrotranscription diagram
enum RetroTranscriptionStates {
    start,
    function_index,
    leaf_indentifier,
    leaf_value,
    end,
};

void normalizeVector(std::vector<double>& v, EncodedPhenotypeType output_type = scoreF, size_t read_index = 0) {
    // State machine to solve non-deterministic generative gramatic
    // for genotype retrotranscription.

    std::stack<double> open_function_indexes;
    RetroTranscriptionStates machine_state = start;
    size_t read_position = 0;

    double current_function_index;
    std::vector<EncodedPhenotypeType> current_function_parameters;
    size_t current_function_parameter_index = 0;

    bool ready = false;
    static const size_t max_output_size = 1024;

    bool current_function_has_leaves;

    while (!ready && read_position < max_output_size) {
        switch (machine_state) {
            case start:
                v[read_position] = 1;
                machine_state = function_index;
                break;
            case function_index:
                // current_function_index = getClosestKey(function_type_dictionary[output_type], v[read_position]);
                v[read_position] = current_function_index;
                open_function_indexes.push(current_function_index);
                current_function_parameters = available_functions[current_function_index].getParamTypes();
                current_function_has_leaves = current_function_parameters.size() != 0;
                machine_state = current_function_has_leaves ? leaf_indentifier : end;
                break;
            case leaf_indentifier:
                v[read_position] = leafTypeToNormalizedValue(available_functions[v[read_position]].getParamTypes()[current_function_parameter_index]);
                ++current_function_parameter_index;
            default:
                throw std::runtime_error(ErrorCodes::INVALID_ENUM_VALUE);
        }
        ++read_position;
    }





}