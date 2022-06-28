#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "errorCodes.hpp"
#include "utils.hpp"

#include <ostream>
#include <sstream>
#include <stdexcept>

// Parameter mappers

typedef struct {
    std::function<double(double)> encoder;
    std::function<double(double)> decoder;
} ParameterMapper;

static const ParameterMapper defaultMapper = {
    .encoder = [](double a){ return a; },
    .decoder = [](double a){ return a; }
};
static const std::map<EncodedPhenotypeType, ParameterMapper> mappers = {
    { durationF, {
        .encoder = [](double s){ return (log10(s) + 6 * log10(2)) / (10 * log10(2)); },
        .decoder = [](double p){ return pow(2, 10 * p - 6); } // In progress
    }},
    { noteValueF, {
        .encoder = [](double v){ return v < 0.003907 ? 0 : (log10(v) + 8 * log10(2)) / (10 * log10(2)); },
        .decoder = [](double p){ return pow(10 * p - 8, 2); }
    }},
    { midiPitchF, {
        .encoder = [](double m){ return m / 127; },
        .decoder = [](double p){ return 127 * p; }
    }},
    { frequencyF, {
        .encoder = [](double f){ return pow(f / 20000, 1/4); },
        .decoder = [](double p){ return 20000 * pow(p, 4); }
    }},
    { articulationF, {
        .encoder = [](double a){ 
            if (a <= 10000) 
                return 0.63662 * atan(1.20416 * sqrt(a * 0.01));
            return 0.998; 
        },
        .decoder = [](double p){ return exp(3 * p); }
    }},
    { intensityF, {
        .encoder = [](double i){ return i / 100; },
        .decoder = [](double p){ return 100 * p; }
    }},
    { goldenintegerF, {
        .encoder = integerToNormalized,
        .decoder = normalizedToInteger,
    }},
    { quantizedF, {
        .encoder = [](int z){ return round((asin(pow(2 * z - 1, 17.0 / 11)) / PI + 0.5) * 72 - 36); },
        .decoder = [](double p){ 
            // tmp
            static const std::map<double, int> LookUpTable({
                { 0, -36 },
                { 0.0005, -35 },
                { 0.001, -34 },
                { 0.003, -33 },
                { 0.006, -32 },
                { 0.008, -31 },
                { 0.01, -30 },
                { 0.015, -29 },
                { 0.02, -28 },
                { 0.025, -27 },
                { 0.03, -26 },
                { 0.04, -25 },
                { 0.045, -24 },
                { 0.05, -23 },
                { 0.06, -22 },
                { 0.07, -21 },
                { 0.08, -20 },
                { 0.09, -19 },
                { 0.1, -18 },
                { 0.11, -17 },
                { 0.12, -16 },
                { 0.14, -15 },
                { 0.15, -14 },
                { 0.16, -13 },
                { 0.18, -12 },
                { 0.2, -11 },
                { 0.21, -10 },
                { 0.23, -9 },
                { 0.25, -8 },
                { 0.27, -7 },
                { 0.3, -6 },
                { 0.32, -5 },
                { 0.33, -4 },
                { 0.36, -3 },
                { 0.4, -2 },
                { 0.45, -1 },
                { 0.5, 0 },
                { 0.55, 1 },
                { 0.6, 2 },
                { 0.64, 3 },
                { 0.67, 4 },
                { 0.68, 5 },
                { 0.7, 6 },
                { 0.73, 7 },
                { 0.75, 8 },
                { 0.77, 9 },
                { 0.79, 10 },
                { 0.8, 11 },
                { 0.82, 12 },
                { 0.84, 13 },
                { 0.85, 14 },
                { 0.86, 15 },
                { 0.88, 16 },
                { 0.89, 17 },
                { 0.9, 18 },
                { 0.91, 19 },
                { 0.92, 20 },
                { 0.93, 21 },
                { 0.94, 22 },
                { 0.95, 23 },
                { 0.955, 24 },
                { 0.96, 25 },
                { 0.97, 26 },
                { 0.975, 27 },
                { 0.98, 28 },
                { 0.985, 29 },
                { 0.99, 30 },
                { 0.992, 31 },
                { 0.994, 32 },
                { 0.997, 33 },
                { 0.999, 34 },
                { 0.9995, 35 },
                { 1, 36 },
            });

            auto it = LookUpTable.begin();
            double min_distance = abs(it -> first - p);
            double current_distance;
            it++;
            for (; it != LookUpTable.end(); it++ ) {
                current_distance = abs(it -> first - p);
                if (current_distance > min_distance) {
                    break;
                }
                min_distance = current_distance;
            } 
            it--;
            return it -> second;
        }
    }}
};

static const std::vector<EncodedPhenotypeType> parameterTypesWithUniformTransformation = {
    noteValueF,
    durationF,
    midiPitchF,
    frequencyF,
    articulationF,
    intensityF,
    quantizedF,
};

double normalizedToUniform(double x) {
    if (x == 0) return 0;
    if (x == 1) return 1;
    return -(pow(E,(14 * x)))/(-1096.63 - pow(E,(14 * x)));
}

double uniformToNormalized(double x) {
    if (x < 0.000912) return 0;
    if (x > 0.999088) return 1;
    return 0.5 + log(x / (1 - x)) / 14;
}

double encodeParameter(EncodedPhenotypeType parameterType, double value) {
    const double encoded_parameter_value = findWithDefault(mappers, parameterType, defaultMapper).encoder(value);
    const double optionally_uniformed_value = 
        includes(parameterTypesWithUniformTransformation, parameterType) 
            ? 
            normalizedToUniform(encoded_parameter_value) 
            : 
            encoded_parameter_value;

    return roundTo6Decimals(optionally_uniformed_value);
}

double decodeParameter(EncodedPhenotypeType parameterType, double encoded_value) {
    const double optionally_deuniformed_value =
        includes(parameterTypesWithUniformTransformation, parameterType) 
            ? 
            uniformToNormalized(encoded_value) 
            : 
            encoded_value;
    
    const double decoded_value = findWithDefault(mappers, parameterType, defaultMapper).decoder(optionally_deuniformed_value);
    
    return roundTo6Decimals(decoded_value);
}

// Utils

std::function<enc_phen_t(std::vector<enc_phen_t>)> 
    buildParameterComputeFunction(EncodedPhenotypeType parameterType, std::string name) {
        if (!isEncodedPhenotypeTypeAParameterType(parameterType)) {
            throw std::runtime_error(ErrorCodes::INVALID_CALL);
        }

        // TO DO: mapper is not correctly initialized
        
        return [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            const double encoded_parameter_value = encodeParameter(parameterType, params[0].getLeafValue());
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
    .index = 1,
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
    .param_types = { leafF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Voice(params);
    },
}),

s({
    .name = "s",
    .index = 4,
    .param_types = { leafF },
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
    .compute = buildParameterComputeFunction(noteValueF, "n"),
}),

d({
    .name = "d",
    .index = 6,
    .param_types = { leafF },
    .output_type = durationF,
    .compute = buildParameterComputeFunction(durationF, "d"),
}),

m({
    .name = "m",
    .index = 7,
    .param_types = { leafF },
    .output_type = midiPitchF,
    .compute = buildParameterComputeFunction(midiPitchF, "m"),
}),

a({
    .name = "a",
    .index = 9,
    .param_types = { leafF },
    .output_type = articulationF,
    .compute = buildParameterComputeFunction(articulationF, "a"),
}),

i({
    .name = "i",
    .index = 10,
    .param_types = { leafF },
    .output_type = intensityF,
    .compute = buildParameterComputeFunction(intensityF, "i"),
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
    .param_types = { quantizedF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return GTree::evaluateAutoReference(eventF, (size_t) params[0].getLeafValue());
    },
}),

vAutoRef({
    .name = "vAutoRef",
    .index = 28,
    .param_types = { quantizedF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return GTree::evaluateAutoReference(eventF, (size_t) params[0].getLeafValue());
    },
}),

sAddV({
    .name = "sAddV",
    .index = 109,
    .param_types = { scoreF, voiceF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        auto& score = params[0], new_voice = params[1];

        std::vector<enc_phen_t> voices = score.getChildren();
        voices.push_back(new_voice);
        return Score(voices);
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
    static bool is_initialized = false;

    if (is_initialized) 
        return;

    is_initialized = true;

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