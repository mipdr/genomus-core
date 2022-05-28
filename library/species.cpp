#include "species.hpp"
#include <algorithm>
#include <stdexcept>

string eventParameterTypeToString(EventParameterType pt) {
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
            throw runtime_error(error_message);
    }
}

Species::Species() {
    this -> _name = "default species";
    this -> _parameter_types = {};
}

Species::Species(SpeciesInitializer init) {
    if (!init.name.length()) {
        throw runtime_error("Cannot asign empty name to species.");
    }

    if (!init.parameter_types.size()) {
        throw runtime_error("Cannot assign empty parameter types vector to species.");
    }

    this -> _name = init.name;
    this -> _parameter_types = init.parameter_types;
}

string Species::toString() {
    string ret = "---SPECIES---";
    ret += "\n\t_name: " + this -> _name;
    ret += "\n\t_parameter_types: (";
    
    for_each(this -> _parameter_types.begin(), this -> _parameter_types.end(), [&](EventParameterType pt) {
        ret += eventParameterTypeToString(pt) + ", ";
    });

    return ret.substr(0, ret.length() - 2) + ")";
}

Species piano = Species({
    .name = "piano",
    .parameter_types = {duration}
});

Species CURRENT_SPECIES = piano;