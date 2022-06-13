#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "species.hpp"
#include "utils.hpp"
#include "errorCodes.hpp"

static const std::map<EncodedPhenotypeType, EncodedPhenotypeType> packed_unpacked_pairs({
    { ept_parameters, ept_parameter },
    { ept_events, ept_event },
    { ept_voices, ept_voice },
});

bool isPackedType(EncodedPhenotypeType eptt) {
    return packed_unpacked_pairs.find(eptt) != packed_unpacked_pairs.end();
}

EncodedPhenotypeType getUnpackedVersion(EncodedPhenotypeType eptt) {
    if (!isPackedType(eptt)) { throw std::runtime_error(ErrorCodes::INVALID_CALL); }
    return packed_unpacked_pairs.find(eptt) -> second;
}

bool areParameterTypeListsCompatible(const std::vector<EncodedPhenotypeType>& packed, const std::vector<EncodedPhenotypeType>& unpacked) {
    // { ept_event, ept_event } is equivalent to { ept_events }
    if (std::any_of(unpacked.begin(), unpacked.end(), [](auto eptt) { return isPackedType(eptt); })) {
        std::cerr << "Only unpacked ept types allowed here\n";
        throw std::runtime_error(ErrorCodes::INVALID_CALL);
    }

    if (packed.size() > unpacked.size()) {
        return false;
    }

    if (packed == unpacked) {
        return true;
    }

    auto packed_it = packed.begin();
    auto unpacked_it = unpacked.begin();
    bool packed_validated = false;
    while (packed_it != packed.end() && unpacked_it != unpacked.end()) {
        if (*packed_it == *unpacked_it) {
            packed_it++;
            unpacked_it++;
        } else if (isPackedType(*packed_it)) {
            if (getUnpackedVersion(*packed_it) == *unpacked_it) {
                packed_validated = true;
                unpacked_it++;
            } else if (packed_validated) {
                packed_validated = false;
                packed_it++;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    if (packed_it == packed.end() && unpacked_it == unpacked.end()) {
        return true;
    }

    if (packed_it == packed.end()) {
        return false;
    }

    if (unpacked_it == unpacked.end()) {
        packed_it++;
        if (packed_validated && packed_it == packed.end()) {
            return true;
        }

        return false;
    }

    // never happens
    return false;
}

// GFunction method implementation

GFunction::GFunction(){ 
    this -> _name = "Not initialized decoded_genotype_level_function";
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = std::vector<EncodedPhenotypeType>();
    this -> _compute = [](std::vector<enc_phen_t> x) -> enc_phen_t { return Parameter(-1.0); };
    this -> _output_type = ept_event;
}

GFunction::GFunction(const GFunction& gf) {
    this -> _name = gf._name;
    this -> _type = gf._type;
    this -> _param_types = gf._param_types;
    this -> _compute = gf._compute;
    this -> _output_type = gf._output_type;
}

GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _build_explicit_form = init.build_explicit_form;
    this -> _output_type = init.output_type;
}

void GFunction::_assert_parameter_format(const std::vector<enc_phen_t>& arg) {
    std::vector<EncodedPhenotypeType> arg_types;
    for_each(arg.begin(), arg.end(), [&](enc_phen_t argument) { arg_types.push_back(argument.getType()); });
    if (!areParameterTypeListsCompatible(this -> _param_types, arg_types)) {
        throw std::runtime_error(ErrorCodes::BAD_GFUNCTION_PARAMETERS);
    }
}

EncodedPhenotypeType GFunction::getOutputType() { return this -> _output_type; }

enc_phen_t GFunction::evaluate(const std::vector<enc_phen_t>& arg) { 
    this -> _assert_parameter_format(arg);
    return this -> _compute(arg); 
}
enc_phen_t GFunction::operator()(const std::vector<enc_phen_t>& arg) { return this -> evaluate(arg); }

std::string GFunction::toString() { 
    std::string ret = "--- GFunction object ---";

    ret += "\n\t_name: " + this -> getName();
    ret += "\n\t_type: " + this -> getTypeString();
    ret += "\n\t_param_types: ";
    for_each(this -> _param_types.begin(), this -> _param_types.end(), [&ret](EncodedPhenotypeType ft){ ret += EncodedPhenotypeTypeToString(ft) + ", "; });
    ret = ret.substr(0, ret.length() - 2) + ";";

    ret += "\n\t_output_type: " + EncodedPhenotypeTypeToString(this -> _output_type) + ";";

    return ret + "\n";
}

std::vector<EncodedPhenotypeType> GFunction::getParamTypes() { return this -> _param_types; }
// function<string(vector<string>)>& GFunction::getBuildExplicitForm() { return this -> _build_explicit_form; }
std::string GFunction::buildExplicitForm(std::vector<std::string> v) {
    return this -> _build_explicit_form(v);
}


// GTree method implementation

GTree::GTree(GFunction& function, std::vector<GTree*> children, float leaf_value): _function(function) {
    this -> _children = children;
    this -> _leaf_value = leaf_value;
}

enc_phen_t GTree::evaluate() {
    if (this -> _function.getOutputType() == ept_parameter) {
        return this -> _function({
            EncodedPhenotype({
                .type = ept_leaf,
                .child_type = ept_leaf,
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
    if (this -> _function.getOutputType() == ept_parameter) {
        return "parameter(to do)";
    }

    std::vector<std::string> string_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree* child_ptr) { 
        string_children.push_back(child_ptr -> toString()); 
    });
    
    return this -> _function.buildExplicitForm(string_children);
}

// GFunction instances
GFunction eventF, paramF, voiceF;

void initialize_dec_gen_lvl_functions() {

    eventF = GFunction({
        .name = "eventF",
        .param_types = { ept_parameters },
        .output_type = ept_event,
        .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
            return Event(params);
        },
        .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
            return std::string("eventF(") + join(children, ", ") + ")"; 
        }
    });

    paramF = GFunction({
        .name = "paramF",
        .param_types = { ept_leaf },
        .output_type = ept_parameter,
        .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
            return Parameter(params);
        },
        .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
            return "paramF()"; 
        }
    });

    voiceF = GFunction({
        .name = "voiceF",
        .param_types = { ept_events },
        .output_type = ept_voice,
        .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
            return Voice(params);
        },
        .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
            return "voiceF(" + join(children, ", ") + ")"; 
        }
    });
}
