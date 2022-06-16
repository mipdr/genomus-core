#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "parameter_mapping.hpp"
#include "species.hpp"
#include "utils.hpp"
#include "errorCodes.hpp"

// static const std::map<EncodedPhenotypeType, EncodedPhenotypeType> packed_unpacked_pairs({
//     { paramFs, paramF },
//     { eventFs, eventF },
//     { ept_voices, ept_voice },
// });

// bool isPackedType(EncodedPhenotypeType eptt) {
//     return packed_unpacked_pairs.find(eptt) != packed_unpacked_pairs.end();
// }

// EncodedPhenotypeType getUnpackedVersion(EncodedPhenotypeType eptt) {
//     if (!isPackedType(eptt)) { throw std::runtime_error(ErrorCodes::INVALID_CALL); }
//     return packed_unpacked_pairs.find(eptt) -> second;
// }

// bool areParameterTypeListsCompatible(const std::vector<EncodedPhenotypeType>& packed, const std::vector<EncodedPhenotypeType>& unpacked) {
//     // { eventF, eventF } is equivalent to { eventFs }
//     if (std::any_of(unpacked.begin(), unpacked.end(), [](auto eptt) { return isPackedType(eptt); })) {
//         std::cerr << "Only unpacked ept types allowed here\n";
//         throw std::runtime_error(ErrorCodes::INVALID_CALL);
//     }

//     if (packed.size() > unpacked.size()) {
//         return false;
//     }

//     if (packed == unpacked) {
//         return true;
//     }

//     auto packed_it = packed.begin();
//     auto unpacked_it = unpacked.begin();
//     bool packed_validated = false;
//     while (packed_it != packed.end() && unpacked_it != unpacked.end()) {
//         if (*packed_it == *unpacked_it) {
//             packed_it++;
//             unpacked_it++;
//         } else if (isPackedType(*packed_it)) {
//             if (getUnpackedVersion(*packed_it) == *unpacked_it) {
//                 packed_validated = true;
//                 unpacked_it++;
//             } else if (packed_validated) {
//                 packed_validated = false;
//                 packed_it++;
//             } else {
//                 return false;
//             }
//         } else {
//             return false;
//         }
//     }

//     if (packed_it == packed.end() && unpacked_it == unpacked.end()) {
//         return true;
//     }

//     if (packed_it == packed.end()) {
//         return false;
//     }

//     if (unpacked_it == unpacked.end()) {
//         packed_it++;
//         if (packed_validated && packed_it == packed.end()) {
//             return true;
//         }

//         return false;
//     }

//     // never happens
//     return false;
// }

// GTree::GFunction method implementation

std::vector<GTree> GTree::tree_nodes;

GTree::GFunction::GFunction(){ 
    this -> _name = "Not initialized decoded_genotype_level_function";
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = std::vector<EncodedPhenotypeType>();
    this -> _compute = [](std::vector<enc_phen_t> x) -> enc_phen_t { return Parameter(-1.0); };
    this -> _output_type = leafF;
}

GTree::GFunction::GFunction(const GTree::GFunction& gf) {
    this -> _name = gf._name;
    this -> _type = gf._type;
    this -> _param_types = gf._param_types;
    this -> _compute = gf._compute;
    this -> _output_type = gf._output_type;
}

GTree::GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _build_explicit_form = init.build_explicit_form;
    this -> _output_type = init.output_type;
}

void GTree::GFunction::_assert_parameter_format(const std::vector<enc_phen_t>& arg) {
    std::vector<EncodedPhenotypeType> arg_types;
    for_each(arg.begin(), arg.end(), [&](enc_phen_t argument) { arg_types.push_back(argument.getType()); });
    // if (!areParameterTypeListsCompatible(this -> _param_types, arg_types)) {
    //     throw std::runtime_error(ErrorCodes::BAD_GTree::GFunction_PARAMETERS);
    // }
    if (this -> _param_types != arg_types) {
        throw std::runtime_error(ErrorCodes::BAD_GFUNCTION_PARAMETERS);
    }
}

EncodedPhenotypeType GTree::GFunction::getOutputType() { return this -> _output_type; }

enc_phen_t GTree::GFunction::evaluate(const std::vector<enc_phen_t>& arg) { 
    this -> _assert_parameter_format(arg);
    return this -> _compute(arg); 
}

enc_phen_t GTree::GFunction::operator()(const std::vector<enc_phen_t>& arg) { return this -> evaluate(arg); }

GTree* GTree::GFunction::operator()(const std::vector<GTree*> children) {
    std::vector<GTree*> ptr_v;

    std::for_each(children.begin(), children.end(), [&](GTree* gt) { ptr_v.push_back(gt); });

    GTree::tree_nodes.push_back(GTree(
        *this,
        ptr_v,
        0
    ));

    return &GTree::tree_nodes[tree_nodes.size() - 1];
}

std::string GTree::GFunction::toString() { 
    std::string ret = "--- GTree::GFunction object ---";

    ret += "\n\t_name: " + this -> getName();
    ret += "\n\t_type: " + this -> getTypeString();
    ret += "\n\t_param_types: ";
    for_each(this -> _param_types.begin(), this -> _param_types.end(), [&ret](EncodedPhenotypeType ft){ ret += EncodedPhenotypeTypeToString(ft) + ", "; });
    ret = ret.substr(0, ret.length() - 2) + ";";

    ret += "\n\t_output_type: " + EncodedPhenotypeTypeToString(this -> _output_type) + ";";

    return ret + "\n";
}

std::vector<EncodedPhenotypeType> GTree::GFunction::getParamTypes() { return this -> _param_types; }
// function<string(vector<string>)>& GTree::GFunction::getBuildExplicitForm() { return this -> _build_explicit_form; }
std::string GTree::GFunction::buildExplicitForm(std::vector<std::string> v) {
    return this -> _build_explicit_form(v);
}


// GTree method implementation

GTree::GTree(GTree::GFunction& function, std::vector<GTree*> children, float leaf_value): _function(function) {
    this -> _children = children;
    this -> _leaf_value = leaf_value;
}

enc_phen_t GTree::evaluate() {
    if (this -> _function.getOutputType() == paramF) {
        return this -> _function({
            EncodedPhenotype({
                .type = leafF,
                .child_type = leafF,
                .children = {},
                .to_string = [](std::vector<std::string>) { return "# to do #"; },
                .leaf_value = this -> _leaf_value,
            })
        });
    }
    // Future work: store evaluation so that each node is only evaluated once.
    std::vector<enc_phen_t> evaluated_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree* child_ptr) { evaluated_children.push_back(child_ptr -> evaluate()); });
    return this -> _function(evaluated_children);
}

std::string GTree::toString() {
    std::cout << "AQUIIII" << std::endl;
    if (this -> _function.getOutputType() == paramF) {
        return "parameter(to do)";
    }

    std::vector<std::string> string_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree* child_ptr) { 
        string_children.push_back(child_ptr -> toString()); 
    });
    
    return this -> _function.buildExplicitForm(string_children);
}

bool isEncodedPhenotypeTypeAParameterType(EncodedPhenotypeType eptt) {
    static const std::vector<EncodedPhenotypeType> parameterTypes({
        noteValueF,
        durationF,
        midiPitchF,
        frequencyF,
        articulationF,
        intensityF,
    });
    return includes(parameterTypes, eptt);
}

std::function<enc_phen_t(std::vector<enc_phen_t>)> 
    buildParameterComputeFunction(EncodedPhenotypeType parameterType, std::string name) {
        if (!isEncodedPhenotypeTypeAParameterType(parameterType)) {
            throw std::runtime_error(ErrorCodes::INVALID_CALL);
        }
        
        return [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            return EncodedPhenotype({
                .type = parameterType,
                .child_type = paramF,
                .children = params,
                .to_string = [=](std::vector<std::string> children_strings) { return name + "(" + children_strings[0] + ")"; },
                .leaf_value = 0,
            });
        };
    };

// GTree::GFunction instances

GTree::GFunction

p({
    .name = "p",
    .param_types = { leafF },
    .output_type = paramF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Parameter(params[0].getLeafValue());
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string {
        return "p(" + children[0] + ")";
    },
}),

e_piano({
    .name = "e_piano",
    .param_types = { noteValueF, midiPitchF, articulationF, intensityF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Event(params);
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return std::string("e(") + join(children, ", ") + ")"; 
    },
}),

v({
    .name = "v",
    .param_types = { eventF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Voice(params);
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "v(" + join(children, ", ") + ")"; 
    }
}),

s({
    .name = "s",
    .param_types = { voiceF },
    .output_type = scoreF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Score(params);
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "s(" + join(children, ", ") + ")"; 
    }
}),

n({
    .name = "n",
    .param_types = { paramF },
    .output_type = noteValueF,
    .compute = buildParameterComputeFunction(noteValueF, "n"),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "n(" + children[0] + ")"; 
    }
}),

d({
    .name = "d",
    .param_types = { paramF },
    .output_type = durationF,
    .compute = buildParameterComputeFunction(durationF, "d"),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "d(" + children[0] + ")"; 
    }
}),

m({
    .name = "m",
    .param_types = { paramF },
    .output_type = midiPitchF,
    .compute = buildParameterComputeFunction(midiPitchF, "m"),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "m(" + children[0] + ")"; 
    }
}),

a({
    .name = "a",
    .param_types = { paramF },
    .output_type = articulationF,
    .compute = buildParameterComputeFunction(articulationF, "a"),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "a(" + children[0] + ")"; 
    }
}),

i({
    .name = "i",
    .param_types = { paramF },
    .output_type = intensityF,
    .compute = buildParameterComputeFunction(intensityF, "i"),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "i(" + children[0] + ")"; 
    }
}),

vConcatE({
    .name = "vConcatE",
    .param_types = { eventF, eventF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return Voice(params);
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "vConcatE(" + join(children, ", ") + ")"; 
    }
}),

vConcatV({
    .name = "vConcatV",
    .param_types = { voiceF, voiceF },
    .output_type = voiceF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        std::vector<enc_phen_t> events;
        events.insert(events.end(), params[0].getChildren().begin(), params[0].getChildren().end());
        events.insert(events.end(), params[1].getChildren().begin(), params[1].getChildren().end());
        
        return Voice(events);
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "vConcatE(" + join(children, ", ") + ")"; 
    }
});;

GTree::GFunction e = e_piano;