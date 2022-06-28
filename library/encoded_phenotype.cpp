#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include "encoded_phenotype.hpp"
#include "errorCodes.hpp"
#include "species.hpp"
#include "utils.hpp"

#define ENCODED_PHENOTYPES_TYPE_CHECK

std::string EncodedPhenotypeTypeToString(const EncodedPhenotypeType& ept) {
    switch (ept) {
        case scoreF:
            return "scoreF";
        case voiceF:
            return "voiceF";
        case eventF:
            return "eventF";
        case listF:
            return "listF";
        case paramF:
            return "paramF";
        case leafF:
            return "leafF";
        case noteValueF:
            return "noteValueF";
        case durationF:
            return "durationF";
        case midiPitchF:
            return "midiPitchF";
        case frequencyF:
            return "frequencyF";
        case articulationF:
            return "articulationF";
        case intensityF:
            return "intensityF";
        case goldenintegerF:
            return "goldenintegerF";
        case quantizedF:
            return "quantizedF";
        case lnoteValueF:
            return "lnoteValueF";
        case ldurationF:
            return "ldurationF";
        case lmidiPitchF:
            return "lmidiPitchF";
        case lfrequencyF:
            return "lfrequencyF";
        case larticulationF:
            return "larticulationF";
        case lintensityF:
            return "lintensityF";
        case lgoldenintegerF:
            return "lgoldenintegerF";
        case lquantizedF:
            return "lquantizedF";
        default:
            throw std::runtime_error("Invalid encoded phenotype type.");
    }
}

EncodedPhenotype::EncodedPhenotype(EncodedPhenotype::EncodedPhenotypeInitializer init) {
    this -> _type = init.type;
    this -> _child_type = init.child_type;
    this -> _children = init.children;
    this -> _to_string = init.to_string;
    this -> _leaf_value = init.leaf_value;
}

EncodedPhenotypeType EncodedPhenotype::getType() { return this -> _type; }
EncodedPhenotypeType EncodedPhenotype::getChildType() { return this -> _child_type; }
std::string EncodedPhenotype::toString() { 
    std::vector<std::string> children_strings;
    for (auto child: this -> _children) {
        children_strings.push_back(child.toString());
    }
    return this -> _to_string(children_strings); 
}

const std::vector<EncodedPhenotype>& EncodedPhenotype::getChildren() { return this -> _children; }

double EncodedPhenotype::getLeafValue() { return this -> _leaf_value; }

bool shouldIncludeChildrenSize(EncodedPhenotypeType type) {
    return includes({scoreF, voiceF}, type) || includes(listTypes, type);
}

std::vector<double> EncodedPhenotype::toNormalizedVector() const {
    if (includes(parameterTypes, this -> _type)) {
        return { this -> _leaf_value };
    }

    std::vector<double> result;
    std::vector<double> evaluated_children;

    static const auto 
        encodeAndAddChildren = [](std::vector<double>& inner_result, const std::vector<EncodedPhenotype>& children) -> void {
            for_each(children.begin(), children.end(), [&](const EncodedPhenotype& ept) {
                inner_result += ept.toNormalizedVector();
            });
        };

    if (shouldIncludeChildrenSize(this -> _type)) {
        result = { integerToNormalized(this -> _children.size()) };
    }
    
    encodeAndAddChildren(result, this -> _children);

    return result;
}

EncodedPhenotype Parameter(double value) {
    return EncodedPhenotype({
        .type = paramF, // ept_parameter,
        .child_type = leafF, // ept_leaf:
        .children = {},
        .to_string = [=](std::vector<std::string>) { return std::to_string(value); },
        .leaf_value = value
    });
}

EncodedPhenotype Parameter(std::vector<EncodedPhenotype> parameters) {
    if (parameters.size() != 1) { throw std::runtime_error(ErrorCodes::BAD_ENC_PHEN_CONSTRUCTION_BAD_LEAF_VALUE); }
    if (parameters[0].getType() != leafF) { throw std::runtime_error(ErrorCodes::BAD_ENC_PHEN_CONSTRUCTION_BAD_CHILD_TYPE); }

    const double value = parameters[0].getLeafValue();
    return EncodedPhenotype({
        .type = paramF,
        .child_type = leafF,
        .children = {},
        .to_string = [=](std::vector<std::string>) { return std::to_string(value); },
        .leaf_value = value
    });
}

EncodedPhenotype Event(std::vector<EncodedPhenotype> parameters) {
    #ifdef ENCODED_PHENOTYPES_TYPE_CHECK
        std::string error_message = "Error in typecheck for Event construction:\n";
        bool error = false;

        // if (any_of(parameters.begin(), parameters.end(), [](EncodedPhenotype p) { return p.getType() != paramF; })) {
        //     error = true;
        //     error_message += " - Not all parameters are of type ept_parameter.\n";
        // }

        if (parameters.size() != CURRENT_SPECIES.getParameterTypes().size()) {
            error = true;
            error_message += " - Missmatch on number of parameters.\n";
        }

        if (error) {
            throw std::runtime_error(error_message);
        }
    #endif

    return EncodedPhenotype({
        .type = eventF,
        .child_type = paramF,
        .children = parameters,
        .to_string = [](std::vector<std::string> children_strings) { return "e(" + join(children_strings) + ")"; },
        .leaf_value = -1.0
    });
}

EncodedPhenotype Voice(std::vector<EncodedPhenotype> parameters) {
    #ifdef ENCODED_PHENOTYPES_TYPE_CHECK
        std::string error_message = "Error in typecheck for Voice construction:\n";
        bool error = false;

        if (any_of(parameters.begin(), parameters.end(), [](EncodedPhenotype p) { return p.getType() != eventF; })) {
            error = true;
            error_message += " - Not all parameters are of type ept_parameter.\n";
        }

        if (error) {
            throw std::runtime_error(error_message);
        }
    #endif

    return EncodedPhenotype({
        .type = voiceF,
        .child_type = eventF,
        .children = parameters,
        .to_string = [](std::vector<std::string> children_strings) { return "v(" + join(children_strings, ", ") + ")"; },
        .leaf_value = -1.0
    });
}

EncodedPhenotype Score(std::vector<EncodedPhenotype> parameters) {
    #ifdef ENCODED_PHENOTYPES_TYPE_CHECK
        std::string error_message = "Error in typecheck for Voice construction:\n";
        bool error = false;

        if (any_of(parameters.begin(), parameters.end(), [](EncodedPhenotype p) { return p.getType() != voiceF; })) {
            error = true;
            error_message += ErrorCodes::BAD_ENC_PHEN_CONSTRUCTION_BAD_CHILD_TYPE;
        }

        if (error) {
            throw std::runtime_error(error_message);
        }
    #endif

    return EncodedPhenotype({
        .type = scoreF,
        .child_type = voiceF,
        .children = parameters,
        .to_string = [](std::vector<std::string> children_strings) { return "s(" + join(children_strings) + ")"; },
        .leaf_value = -1.0
    });
}