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
        case ept_parameter:
            return "ept_parameter";
        case ept_event:
            return "ept_event";
        case ept_voice:
            return "ept_voice";
        case ept_score:
            return "ept_score";
        case ept_parameters:
            return "ept_parameters";
        case ept_events:
            return "ept_events";
        case ept_voices:
            return "ept_voices";
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

float EncodedPhenotype::getLeafValue() { return this -> _leaf_value; }

EncodedPhenotype Parameter(float value) {
    return EncodedPhenotype({
        .type = ept_parameter,
        .child_type = ept_leaf,
        .children = {},
        .to_string = [=](std::vector<std::string>) { return std::to_string(value); },
        .leaf_value = value
    });
}

EncodedPhenotype Parameter(std::vector<EncodedPhenotype> parameters) {
    if (parameters.size() != 1) { throw std::runtime_error(ErrorCodes::BAD_ENC_PHEN_CONSTRUCTION_BAD_LEAF_VALUE); }
    if (parameters[0].getType() != ept_leaf) { throw std::runtime_error(ErrorCodes::BAD_ENC_PHEN_CONSTRUCTION_BAD_CHILD_TYPE); }

    const float value = parameters[0].getLeafValue();
    return EncodedPhenotype({
        .type = ept_parameter,
        .child_type = ept_leaf,
        .children = {},
        .to_string = [=](std::vector<std::string>) { return std::to_string(value); },
        .leaf_value = value
    });
}

EncodedPhenotype Event(std::vector<EncodedPhenotype> parameters) {
    #ifdef ENCODED_PHENOTYPES_TYPE_CHECK
        std::string error_message = "Error in typecheck for Event construction:\n";
        bool error = false;

        if (any_of(parameters.begin(), parameters.end(), [](EncodedPhenotype p) { return p.getType() != ept_parameter; })) {
            error = true;
            error_message += " - Not all parameters are of type ept_parameter.\n";
        }

        if (parameters.size() != CURRENT_SPECIES.getParameterTypes().size()) {
            error = true;
            error_message += " - Missmatch on number of parameters.\n";
        }

        if (error) {
            throw std::runtime_error(error_message);
        }
    #endif

    return EncodedPhenotype({
        .type = ept_event,
        .child_type = ept_parameter,
        .children = parameters,
        .to_string = [](std::vector<std::string> children_strings) { return "event(" + join(children_strings) + ")"; },
        .leaf_value = -1.0
    });
}

EncodedPhenotype Voice(std::vector<EncodedPhenotype> parameters) {
    #ifdef ENCODED_PHENOTYPES_TYPE_CHECK
        std::string error_message = "Error in typecheck for Voice construction:\n";
        bool error = false;

        if (any_of(parameters.begin(), parameters.end(), [](EncodedPhenotype p) { return p.getType() != ept_event; })) {
            error = true;
            error_message += " - Not all parameters are of type ept_parameter.\n";
        }

        if (error) {
            throw std::runtime_error(error_message);
        }
    #endif

    return EncodedPhenotype({
        .type = ept_voice,
        .child_type = ept_event,
        .children = parameters,
        .to_string = [](std::vector<std::string> children_strings) { return "voice(\n\t" + join(children_strings, ",\n\t") + "\n)"; },
        .leaf_value = -1.0
    });
}

EncodedPhenotype Score(std::vector<EncodedPhenotype> parameters) {
    #ifdef ENCODED_PHENOTYPES_TYPE_CHECK
        std::string error_message = "Error in typecheck for Voice construction:\n";
        bool error = false;

        if (any_of(parameters.begin(), parameters.end(), [](EncodedPhenotype p) { return p.getType() != ept_voice; })) {
            error = true;
            error_message += ErrorCodes::BAD_ENC_PHEN_CONSTRUCTION_BAD_CHILD_TYPE;
        }

        if (error) {
            throw std::runtime_error(error_message);
        }
    #endif

    return EncodedPhenotype({
        .type = ept_score,
        .child_type = ept_voice,
        .children = parameters,
        .to_string = [](std::vector<std::string> children_strings) { return "score(\n\t\t" + join(children_strings, ",\n\t\t") + "\n)"; },
        .leaf_value = -1.0
    });
}