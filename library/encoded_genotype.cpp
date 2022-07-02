#include "encoded_genotype.hpp"
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "errorCodes.hpp"
#include "utils.hpp"

#include <iostream>
#include <ostream>
#include <random>
#include <stack>
#include <stdexcept>

static const unsigned int MAX_RANDOM_VECTOR_LENGTH = 128;

double randomNormalized() {
    return ((double) rand() / (RAND_MAX));
}

std::vector<double> randomVector(int n) {
    std::vector<double> v;
    auto generator = new std::random_device;

    if (n > MAX_RANDOM_VECTOR_LENGTH) {
        n = MAX_RANDOM_VECTOR_LENGTH;
    }

    for (size_t i = 0; i < n; ++i) {
        v.push_back(randomNormalized());
    }
    return v;
}

std::vector<double> newGerminalVector() {
    size_t size = std::rand() % MAX_RANDOM_VECTOR_LENGTH;
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

static const VectorNormalizationState default_vector_normalization_state = {
    // .position = 0,
    .output_type = scoreF,
    .current_depth = 0,
};

void innerNormalizeVector(std::vector<double>& input, std::vector<double>& output, VectorNormalizationState state) {
    double current_function_index;
    RetroTranscriptionStates machine_state = start;
    static size_t position = 0;
    static size_t read_position = position % input.size();

    std::vector<EncodedPhenotypeType> current_function_parameters;

    bool ready = false;
    bool current_function_has_children;

    static const auto advance = [&]() {
        position++;
        read_position = position % input.size();
    };

    bool limits_overpassed = state.current_depth > MAX_GENOTYPE_DEPTH || position > MAX_GENOTYPE_VECTOR_SIZE;
    FunctionTypeDictionary& dictionary = limits_overpassed ? default_function_type_dictionary : function_type_dictionary;

    while (!ready && position < MAX_GENOTYPE_VECTOR_SIZE) {
        switch (machine_state) {
            case start:
                output.push_back(1);
                machine_state = function_index;
                break;
            case function_index:
                // Find closest type-conforming index
                current_function_index = getClosestValue(dictionary[state.output_type], input[read_position]);
                output.push_back(current_function_index);
                advance();

                current_function_parameters = available_functions[current_function_index].getParamTypes();

                if (isEncodedPhenotypeTypeAParameterType(state.output_type)) {
                    // Go for leaf parameter
                    output.push_back(leafTypeToNormalizedValue(state.output_type));
                    advance();
                    output.push_back(input[read_position]);
                } else if (current_function_parameters.size()) {
                    // Explore parameter types and compute parameters on output
                    for (auto parameterType : current_function_parameters) {
                        innerNormalizeVector(input, output, {
                            .output_type = parameterType,
                            .current_depth = state.current_depth + 1,
                        });
                    }
                }

                machine_state = end;
                break;
            case leaf_indentifier:
                // output[write_position] = leafTypeToNormalizedValue(available_functions[input[read_position]].getParamTypes()[current_function_parameter_index]);
                // ++current_function_parameter_index;
            case end:
                output.push_back(0);
                ready = true;
                break;
            default:
                throw std::runtime_error(ErrorCodes::INVALID_ENUM_VALUE);
        }
        advance();
    }
}

void normalizeVector(std::vector<double>& input, std::vector<double>& output) {
    innerNormalizeVector(input, output, default_vector_normalization_state);
}