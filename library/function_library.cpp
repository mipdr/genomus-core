#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "errorCodes.hpp"
#include "utils.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

std::function<enc_phen_t(std::vector<enc_phen_t>)> 
    buildParameterComputeFunction(EncodedPhenotypeType parameterType, std::string name, std::function<double(double)> parameter_encoder) {
        if (!isEncodedPhenotypeTypeAParameterType(parameterType)) {
            throw std::runtime_error(ErrorCodes::INVALID_CALL);
        }

        // TO DO: mapper is not correctly initialized
        
        return [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            const double encoded_parameter_value = parameter_encoder(params[0].getLeafValue());
            return EncodedPhenotype({
                .type = parameterType,
                .child_type = leafF,
                .children = params,
                .to_string = [=](std::vector<std::string> children_strings) { return name + "(" + std::to_string(encoded_parameter_value) + ")"; },
                .leaf_value = encoded_parameter_value,
            });
        };
    };

std::map<std::string, std::string> name_aliases;

// GTree::GFunction instances

GTree::GFunction

p({
    .name = "p",
    .index = 0,
    .param_types = { leafF },
    .output_type = paramF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Parameter(params[0].getLeafValue());
    },
}),

e_piano({
    .name = "e_piano",
    .index = 2,
    .param_types = { noteValueF, midiPitchF, articulationF, intensityF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Event(params);
    },
}),

v({
    .name = "v",
    .index = 3,
    .param_types = { eventF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Voice(params);
    },
}),

s({
    .name = "s",
    .index = 4,
    .param_types = { voiceF },
    .output_type = scoreF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Score(params);
    },
}),

n({
    .name = "n",
    .index = 5,
    .param_types = { leafF },
    .output_type = noteValueF,
    .compute = buildParameterComputeFunction(noteValueF, "n", [](double v){ return (log(v) + 8 * log(2)) / 10 * log(2); }),
}),

d({
    .name = "d",
    .index = 6,
    .param_types = { leafF },
    .output_type = durationF,
    .compute = buildParameterComputeFunction(durationF, "d", [](double p){ return pow(10 * p - 6, 2); }),
}),

m({
    .name = "m",
    .index = 7,
    .param_types = { leafF },
    .output_type = midiPitchF,
    .compute = buildParameterComputeFunction(midiPitchF, "m", [](double m){ return (m - 12) / 100; }),
}),

a({
    .name = "a",
    .index = 9,
    .param_types = { leafF },
    .output_type = articulationF,
    .compute = buildParameterComputeFunction(articulationF, "a", [](double a){ return pow(a / 3, 1 / E); }),
}),

i({
    .name = "i",
    .index = 10,
    .param_types = { leafF },
    .output_type = intensityF,
    .compute = buildParameterComputeFunction(intensityF, "i", [](double i){ return i / 127; }),
}),

vConcatE({
    .name = "vConcatE",
    .index = 42,
    .param_types = { eventF, eventF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Voice(params);
    },
}),

vConcatV({
    .name = "vConcatV",
    .index = 43,
    .param_types = { voiceF, voiceF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        std::vector<enc_phen_t> events;
        events.insert(events.end(), params[0].getChildren().begin(), params[0].getChildren().end());
        events.insert(events.end(), params[1].getChildren().begin(), params[1].getChildren().end());
        
        return Voice(events);
    },
}),

eAutoRef({
    .name = "eAutoRef",
    .index = 27,
    .param_types = { leafF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return GTree::evaluateAutoReference(eventF, (size_t) params[0].getLeafValue());
    },
}),

vAutoRef({
    .name = "vAutoRef",
    .index = 28,
    .param_types = { leafF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return GTree::evaluateAutoReference(eventF, (size_t) params[0].getLeafValue());
    },
}),

e = e_piano.alias("e");

std::map<double, GTree::GFunction> available_functions;
std::map<EncodedPhenotypeType, std::vector<double>> function_type_dictionary;
std::map<std::string, double> function_name_to_index;

double encodeIndex(size_t index) {
    auto aux = index * PHI;
    return roundTo6Decimals(aux - (int)aux);
}

std::string print_available_functions() {
    std::stringstream ss;

    for (auto entry: available_functions) { 
        ss << "ENTRY: " << entry.first << '\n';
        ss << entry.second.toString() << '\n';
    }

    return ss.str();
}

std::string print_function_type_dictionary() {
    std::stringstream ss;

    ss << "FUNCTION DICTIONARY:" << '\n';
    for (auto entry: function_type_dictionary) {
        ss << '\t' << EncodedPhenotypeTypeToString(entry.first) << ": " << join(entry.second) << '\n';
    }

    return ss.str();
}

void init_available_functions() {
    double encoded_index;
    bool isAlias;
    for (auto gf: { GENOTYPE_FUNCTIONS }) {
        isAlias = name_aliases.find(gf.getName()) != name_aliases.end();

        if (!isAlias) {
            encoded_index = encodeIndex(gf.getIndex());
            if (available_functions.find(encoded_index) != available_functions.end()) {
                throw std::runtime_error(ErrorCodes::ALREADY_EXISTING_FUNCTION_INDEX + ": " + gf.getName());
            }
            available_functions[encodeIndex(gf.getIndex())] = gf;
            function_type_dictionary[gf.getOutputType()].push_back(encoded_index);
            function_name_to_index[gf.getName()] = encoded_index;
        }

    }
}