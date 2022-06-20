#include "species.hpp"
#include "encoded_phenotype.hpp"
#include <algorithm>
#include <stdexcept>

std::string ParameterTypeToString(ParameterType pt) {
    switch (pt) {
        case duration:
            return "duration";
        case pitch:
            return "pitch";
        case articulation:
            return "articulation";
        case intensity:
            return "intensity";
        default:
            char* error_message;
            snprintf(error_message, 50, "Invalid event parameter type: %d", (int)pt);
            throw std::runtime_error(error_message);
    }
}

Species::Species() {
    this -> _name = "default species";
    this -> _parameter_types = {};
}

Species::Species(SpeciesInitializer init) {
    if (!init.name.length()) {
        throw std::runtime_error("Cannot asign empty name to species.");
    }

    if (!init.parameter_types.size()) {
        throw std::runtime_error("Cannot assign empty parameter types vector to species.");
    }

    this -> _name = init.name;
    this -> _parameter_types = init.parameter_types;
}

std::vector<ParameterType> Species::getParameterTypes() { return this -> _parameter_types; }

std::string Species::toString() {
    std::string ret = "---SPECIES---";
    ret += "\n\t_name: " + this -> _name;
    ret += "\n\t_parameter_types: (";
    
    for_each(this -> _parameter_types.begin(), this -> _parameter_types.end(), [&](ParameterType pt) {
        ret += ParameterTypeToString(pt) + ", ";
    });

    return ret.substr(0, ret.length() - 2) + ")";
}

Species piano = Species({
    .name = "piano",
    .parameter_types = { noteValue, duration, pitch, intensity}
});

Species CURRENT_SPECIES = piano;