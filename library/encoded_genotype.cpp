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
#include <string>

static const unsigned int MAX_RANDOM_VECTOR_LENGTH = 128;

double randomNormalized() {
    return ((double) rand() / (RAND_MAX));
}

std::vector<double> randomVector(int n) {
    std::vector<double> v;

    if (n > MAX_RANDOM_VECTOR_LENGTH) {
        n = MAX_RANDOM_VECTOR_LENGTH;
    }

    for (size_t i = 0; i < n; ++i) {
        v.push_back(randomNormalized());
    }
    return v;
}

std::vector<double> newGerminalVector() {
    size_t size = (std::rand() % MAX_RANDOM_VECTOR_LENGTH) + 1;
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

double getClosestFunctionIndex(FunctionTypeDictionary& dictionary, EncodedPhenotypeType type, double value, bool include_autoreferences) {
    // Can return an autoreference function only if there are available subexpressions available.
    double current_function_index = getClosestValue(dictionary[type], value);

    if (autoreference_type_dictionary[type] == current_function_index && !include_autoreferences) {
        current_function_index = getClosestValue(dictionary[type], value, true);
    }

    return current_function_index;
}

void innerNormalizeVector(const std::vector<double>& input, std::vector<double>& output, VectorNormalizationState state) {
    double current_function_index;
    RetroTranscriptionStates machine_state = start;
    static size_t position = 0;
    static std::vector<EncodedPhenotypeType> autoreferenciable_types = {};

    if (state.current_depth == 0) {
        position = 0;
        autoreferenciable_types = {};
    }
    
    static size_t read_position = position % input.size();

    std::vector<EncodedPhenotypeType> current_function_parameters;

    bool ready = false;
    bool current_function_has_children;

    const auto advance = [&]() {
        if (input.size() == 0) {
            throw std::runtime_error("xxx");
        }
        position++;
        read_position = position % input.size();
    };

    bool limits_overpassed = state.current_depth > MAX_GENOTYPE_DEPTH || position > MAX_GENOTYPE_VECTOR_SIZE;
    FunctionTypeDictionary& dictionary = limits_overpassed ? default_function_type_dictionary : function_type_dictionary;

    while (!ready) {
        switch (machine_state) {
            case start:
                output.push_back(1);
                machine_state = function_index;
                advance();
                break;
            case function_index:
                // Find closest type-conforming index
                current_function_index = getClosestFunctionIndex(dictionary, state.output_type, input[read_position], includes(autoreferenciable_types, state.output_type));
                output.push_back(current_function_index);
                advance();

                current_function_parameters = available_functions[current_function_index].getParamTypes();

                if (isEncodedPhenotypeTypeAParameterType(state.output_type) && !available_functions[current_function_index].getIsRandom()) {
                    // Go for leaf parameter
                    output.push_back(leafTypeToNormalizedValue(state.output_type));
                    advance();
                    output.push_back(input[read_position]);
                    advance();
                } else if (isEncodedPhenotypeTypeAListType(state.output_type)) {
                    const double leafTypeMarker = leafTypeToNormalizedValue(listToParameterType(state.output_type));
                    size_t list_size = 0;

                    // Go for list parameters
                    do {
                        output.push_back(leafTypeMarker);
                        advance();
                        output.push_back(input[read_position]);
                        advance();
                        list_size++;
                        if (input[read_position] < 0.5) break;
                    } while (list_size < MAX_LIST_SIZE);
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
            case end:
                output.push_back(0);
                ready = true;

                autoreferenciable_types.push_back(state.output_type);
                advance();
                break;
            default:
                throw std::runtime_error(ErrorCodes::INVALID_ENUM_VALUE);
        }
    }
}

void normalizeVector(const std::vector<double>& input, std::vector<double>& output) {
    innerNormalizeVector(input, output, default_vector_normalization_state);
}

std::string innerToExpression(const std::vector<double>& input, VectorNormalizationState state) {
    // Code mostly reused from normalizeVector. Probably it is possible to unify the two functions.

    double current_function_index;
    RetroTranscriptionStates machine_state = start;
    static size_t position = 0;
    static std::string result = "";

    if (state.current_depth == 0) {
        position = 0;
        result = "";
    }
    
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

    while (!ready) {
        switch (machine_state) {
            case start:
                if (input[read_position] != 1.0) {
                    throw std::runtime_error("Expected 1.0 at position " + std::to_string(read_position));
                }
                machine_state = function_index;
                advance();
                break;
            case function_index:
                // Find closest type-conforming index
                current_function_index = getClosestFunctionIndex(dictionary, state.output_type, input[read_position], true);
                result += available_functions[current_function_index].getName() + "(";
                advance();

                current_function_parameters = available_functions[current_function_index].getParamTypes();

                if (isEncodedPhenotypeTypeAParameterType(state.output_type) && !available_functions[current_function_index].getIsRandom()) {
                    // Go for leaf parameter
                    if (input[read_position] != leafTypeToNormalizedValue(state.output_type)) {
                        throw std::runtime_error("Expected formatted parameter type at position " + std::to_string(read_position));
                    }
                    advance();
                    const double decoded_leaf = decodeParameter(state.output_type, input[read_position]);
                    result += std::to_string(decoded_leaf);
                    advance();
                } else if (isEncodedPhenotypeTypeAListType(state.output_type)) {
                    const double leafTypeMarker = leafTypeToNormalizedValue(listToParameterType(state.output_type));
                    const double associated_parameter_function_index = 
                        default_function_type_dictionary[listToParameterType(state.output_type)][0];
                    const std::string associated_parameter_function_name = 
                        available_functions[associated_parameter_function_index].getName();
                    size_t list_size = 0;

                    // Go for list parameters
                    do {
                        if (input[read_position] != leafTypeMarker) {
                            throw std::runtime_error("Expected formatted parameter type at position " + std::to_string(read_position));
                        }
                        advance();
                        const double decoded_leaf = decodeParameter(listToParameterType(state.output_type), input[read_position]);
                        result += associated_parameter_function_name + "(" + std::to_string(decoded_leaf) + ")" + ", ";
                        advance();
                        list_size++;

                        if (input[read_position] < 0.5){
                            result = result.substr(0, result.length() - 2);
                            break;
                        }
                    } while (list_size < MAX_LIST_SIZE);
                } else if (current_function_parameters.size()) {
                    // Explore parameter types and compute parameters on output
                    for (auto parameterType : current_function_parameters) {
                        innerToExpression(input, {
                            .output_type = parameterType,
                            .current_depth = state.current_depth + 1,
                        });
                        result += ", ";
                    }

                    result = result.substr(0, result.size() - 2);
                }

                machine_state = end;
                break;
            case end:
                if (input[read_position] != 0) {
                    throw std::runtime_error("Expected 0 at position " + std::to_string(read_position));
                }
                result += ")";
                ready = true;
                advance();
                break;
            default:
                throw std::runtime_error(ErrorCodes::INVALID_ENUM_VALUE);
        }
    }

    try {
        if (result == "") {
            throw std::runtime_error("Something went wrong");
        }

    } catch (std::runtime_error e) {

    }


    return result;
}

std::string toExpression(const std::vector<double>& input) {
    return innerToExpression(input, default_vector_normalization_state);
}