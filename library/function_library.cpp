#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "errorCodes.hpp"
#include "utils.hpp"

#include <algorithm>
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
        .decoder = [](double p){ return p < 0.006695 ? 0 : pow(2, 10 * p - 8); }
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
        .decoder = [](double p){ 
            if (p < 0.998) 
                return round((pow(tan(p * PI * 0.5), 2) / 1.45 * 100));
            return 10000.0;
        }
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

GTree::GFunction::GFunctionInitializer buildParameterFunction(std::string name, EncodedPhenotypeType output_type, size_t index) {
    if (!isEncodedPhenotypeTypeAParameterType(output_type)) {
        throw std::runtime_error(ErrorCodes::INVALID_CALL);
    }
    
    return {
        .name = name,
        .index = index,
        .param_types = { leafF },
        .output_type = output_type,
        .compute = [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            const double encoded_parameter_value = encodeParameter(output_type, params[0].getLeafValue());
            return EncodedPhenotype({
                .type = output_type,
                .child_type = leafF,
                .children = params,
                .to_string = [=](std::vector<std::string> children_strings) { return name + "(" + std::to_string(encoded_parameter_value) + ")"; },
                .leaf_value = encoded_parameter_value,
            });
        },
        .default_function_for_type = true,
    };
}

GTree::GFunction::GFunctionInitializer buildListFunction(std::string name, EncodedPhenotypeType output_type, size_t index) {
    if (!isEncodedPhenotypeTypeAListType(output_type)) {
        throw std::runtime_error(ErrorCodes::INVALID_CALL);
    }
    
    return {
        .name = name,
        .index = index,
        .param_types = { listF },
        .output_type = output_type,
        .compute = [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            double encoded_parameter_value;

            for (auto&& param: params) {
                encoded_parameter_value = encodeParameter(output_type, param.getLeafValue());

                param = EncodedPhenotype({
                    .type = listToParameterType(output_type),
                    .child_type = leafF,
                    .children = { param },
                    .to_string = [=](std::vector<std::string> children_strings) { return std::to_string(encoded_parameter_value); },
                    .leaf_value = encoded_parameter_value,
                });
            }

            return EncodedPhenotype({
                .type = output_type,
                .child_type = listToParameterType(output_type),
                .children = params,
                .to_string = [=](std::vector<std::string> children_strings) { 
                    std::vector<std::string> evaluated_children;
                    std::for_each(params.begin(), params.end(), [&](auto param) { evaluated_children.push_back(param.toString()); });
                    return name + "(" + join(evaluated_children) + ")"; 
                },
                .leaf_value = encoded_parameter_value,
            });
        },
        .default_function_for_type = true,
    };
}


GTree::GFunction::GFunctionInitializer buildRandomFunction(std::string name, EncodedPhenotypeType output_type, size_t index) {
    return {
        .name = name,
        .index = index,
        .param_types = {},
        .output_type = output_type,
        .compute = [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            if (params.size() == 0) {
                const double random_number = GTree::RNG.nextDouble();
                return enc_phen_t({
                    .type = output_type,
                    .child_type = leafF,
                    .children = {},
                    .to_string = [=](std::vector<std::string> children_strings) { return name + "(" + std::to_string(random_number) + ")"; },
                    .leaf_value = random_number,
                });
            } else {
                return params[0];
            }
        },
        .is_random = true,
    };
}

std::map<std::string, std::string> name_aliases;

// GTree::GFunction instances

GTree::GFunction

p({
    .name = "p",
    .index = 100,
    .param_types = { leafF },
    .output_type = paramF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Parameter(params[0].getLeafValue());
    },
    .default_function_for_type = true,
}),

e_piano({
    .name = "e_piano",
    .index = 2,
    .param_types = { noteValueF, midiPitchF, articulationF, intensityF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Event(params);
    },
    .default_function_for_type = true,
}),

v({
    .name = "v",
    .index = 3,
    .param_types = { eventF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Voice(params);
    },
    .default_function_for_type = true,
}),

s({
    .name = "s",
    .index = 4,
    .param_types = { voiceF },
    .output_type = scoreF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Score(params);
    },
    .default_function_for_type = true,
}),

n(buildParameterFunction("n", noteValueF, 5)),
d(buildParameterFunction("d", durationF, 6)),
m(buildParameterFunction("m", midiPitchF, 7)),
f(buildParameterFunction("f", frequencyF, 8)),
a(buildParameterFunction("a", articulationF, 9)),
i(buildParameterFunction("i", intensityF, 10)),
z(buildParameterFunction("z", goldenintegerF, 11)),
q(buildParameterFunction("q", quantizedF, 12)),

ln(buildListFunction("ln", lnoteValueF, 15)),
ld(buildListFunction("ld", ldurationF, 16)),
lm(buildListFunction("lm", lmidiPitchF, 17)),
lf(buildListFunction("lf", lfrequencyF, 18)),
la(buildListFunction("la", larticulationF, 19)),
li(buildListFunction("li", lintensityF, 20)),

s2V({
    .name = "s2V",
    .index = 104,
    .param_types = { voiceF, voiceF },
    .output_type = scoreF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Score(params);
    },
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

vMotif({
    .name = "vMotif",
    .index = 199,
    .param_types = { lnoteValueF, lmidiPitchF, larticulationF, lintensityF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        std::vector<enc_phen_t> events;
        size_t min = -1;

        for (auto& param: params) {
            min = std::min(min, param.getChildren().size());
        }

        for (size_t i = 0; i < min; ++i) {
            events.push_back(Event({
                params[0].getChildren()[i],
                params[1].getChildren()[i],
                params[2].getChildren()[i],
                params[3].getChildren()[i],
            }));
        }

        return Voice(events);
    },
}),

vMotifLoop({
    .name = "vMotifLoop",
    .index = 200,
    .param_types = { lnoteValueF, lmidiPitchF, larticulationF, lintensityF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        std::vector<enc_phen_t> events;
        size_t max = 0;

        for (auto& param: params) {
            max = std::max(max, param.getChildren().size());
        }

        for (size_t i = 0; i < max; ++i) {
            events.push_back(Event({
                params[0].getChildren()[i % params[0].getChildren().size()],
                params[1].getChildren()[i % params[1].getChildren().size()],
                params[2].getChildren()[i % params[2].getChildren().size()],
                params[3].getChildren()[i % params[3].getChildren().size()],
            }));
        }

        return Voice(events);
    },
}),

vPerpetuumMobile({
    .name = "vPerpetuumMobile",
    .index = 201,
    .param_types = { noteValueF, lmidiPitchF, larticulationF, lintensityF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        std::vector<enc_phen_t> events;
        size_t min = -1;

        for (auto& param: params) {
            min = std::min(min, param.getChildren().size());
        }

        for (size_t i = 0; i < min; ++i) {
            events.push_back(Event({
                params[0],
                params[1].getChildren()[i],
                params[2].getChildren()[i],
                params[3].getChildren()[i],
            }));
        }

        return Voice(events);
    },
}),

vPerpetuumMobileLoop({
    .name = "vPerpetuumMobile",
    .index = 202,
    .param_types = { noteValueF, lmidiPitchF, larticulationF, lintensityF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        std::vector<enc_phen_t> events;
        size_t max = 0;

        for (auto& param: params) {
            max = std::max(max, param.getChildren().size());
        }

        for (size_t i = 0; i < max; ++i) {
            events.push_back(Event({
                params[0],
                params[1].getChildren()[i % params[1].getChildren().size()],
                params[2].getChildren()[i % params[2].getChildren().size()],
                params[3].getChildren()[i % params[3].getChildren().size()],
            }));
        }

        return Voice(events);
    },
}),

eAutoref({
    .name = "eAutoref",
    .index = 27,
    .param_types = { goldenintegerF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        // Autoreferences must be evaluated by the GTree object, not by the GFunction
        throw std::runtime_error(ErrorCodes::INVALID_CALL);
    },
    .is_Autoreference = true,
}),

vAutoref({
    .name = "vAutoref",
    .index = 28,
    .param_types = { goldenintegerF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        throw std::runtime_error(ErrorCodes::INVALID_CALL);
    },
    .is_Autoreference = true,
}),

sAddV({
    .name = "sAddV",
    .index = 109,
    .param_types = { scoreF, voiceF },
    .output_type = scoreF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        auto& score = params[0], new_voice = params[1];

        std::vector<enc_phen_t> voices = score.getChildren();
        voices.push_back(new_voice);
        return Score(voices);
    },
}),

sAddS({
    .name = "sAddS",
    .index = 110,
    .param_types = { scoreF, scoreF },
    .output_type = scoreF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Score(params[0].getChildren() + params[1].getChildren());
    },
}), 

nRnd(buildRandomFunction("nRnd", noteValueF, 310)),
dRnd(buildRandomFunction("dRnd", durationF, 311)),
mRnd(buildRandomFunction("mRnd", midiPitchF, 312)),
fRnd(buildRandomFunction("fRnd", frequencyF, 313)),
aRnd(buildRandomFunction("aRnd", articulationF, 314)),
iRnd(buildRandomFunction("iRnd", intensityF, 315)),
zRnd(buildRandomFunction("zRnd", goldenintegerF, 316)),
qRnd(buildRandomFunction("qRnd", quantizedF, 317)),



e = e_piano.alias("e");

std::map<double, GTree::GFunction> available_functions;
std::vector<std::string> exclude_functions = { };
FunctionTypeDictionary function_type_dictionary;
FunctionTypeDictionary default_function_type_dictionary;
std::map<EncodedPhenotypeType, double> autoreference_type_dictionary;
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
        ss << '\t' << encodedPhenotypeTypeToString(entry.first) << ": " << join(entry.second) << '\n';
    }

    return ss.str();
}

void init_available_functions() {
    static bool is_initialized = false;

    if (is_initialized) 
        return;

    is_initialized = true;

    double encoded_index;
    bool is_alias, is_excluded;
    for (auto gf: { GENOTYPE_FUNCTIONS }) {
        is_alias = name_aliases.find(gf.getName()) != name_aliases.end();
        is_excluded = find(exclude_functions.begin(), exclude_functions.end(), gf.getName()) != exclude_functions.end();

        if (!is_alias && !is_excluded) {
            encoded_index = encodeIndex(gf.getIndex());
            if (available_functions.find(encoded_index) != available_functions.end()) {
                throw std::runtime_error(ErrorCodes::ALREADY_EXISTING_FUNCTION_INDEX + ": " + gf.getName());
            }
            available_functions[encodeIndex(gf.getIndex())] = gf;
            function_type_dictionary[gf.getOutputType()].push_back(encoded_index);
            function_name_to_index[gf.getName()] = encoded_index;

            if (gf.getIsAutoreference()) {
                if (autoreference_type_dictionary[gf.getOutputType()] != 0) {
                    throw std::runtime_error("There can only be one autoreference function per type.");
                }
                autoreference_type_dictionary[gf.getOutputType()] = encoded_index;
            }

            if (gf.getIsDefaultForType()) {
                default_function_type_dictionary[gf.getOutputType()].push_back(encoded_index);
            }
        }
    }

    // Check correctness of default dictionary
    if (default_function_type_dictionary.size() != function_type_dictionary.size()) {
        throw std::runtime_error("Missing default functions for some types.");
    }

    for (auto [type, v]: default_function_type_dictionary) {
        if (v.size() > 1) {
            std::runtime_error("Found more than one default function for type " + encodedPhenotypeTypeToString(type) + ": " + to_string(v));
        }
    }
}