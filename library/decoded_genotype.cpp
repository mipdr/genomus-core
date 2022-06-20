#include <functional>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "parameter_mapping.hpp"
#include "species.hpp"
#include "utils.hpp"
#include "errorCodes.hpp"

bool isEncodedPhenotypeTypeAParameterType(EncodedPhenotypeType eptt) {
    static const std::vector<EncodedPhenotypeType> parameterTypes({
        noteValueF,
        durationF,
        midiPitchF,
        frequencyF,
        articulationF,
        intensityF,
        paramF,
    });
    return includes(parameterTypes, eptt);
}

bool gfunctionAcceptsFloatParameter(const GTree::GFunction& gf) {
    return gf.getIsAutoreference() || isEncodedPhenotypeTypeAParameterType(gf.getOutputType());
}

// GTree::GTreeIndex method implementation

GTree::GTreeIndex::GTreeIndex(size_t i) { this -> _index = i; }
enc_phen_t GTree::GTreeIndex::evaluate(){ return tree_nodes[this -> _index].evaluate(); }
std::string GTree::GTreeIndex::toString(){ 
    return tree_nodes[this -> _index].toString(); 
}
GTree::GTreeIndex::operator size_t() const { return this -> _index; }
float GTree::GTreeIndex::getLeafValue() {
    return tree_nodes[this -> _index]._leaf_value;
}
size_t GTree::GTreeIndex::getIndex() { return this -> _index; }

void GTree::GTreeIndex::clean() {
    GTree::clean();
}

EncodedPhenotype GTree::evaluateAutoReference(EncodedPhenotypeType eptt, size_t index) {
    std::vector<GTree::GTreeIndex> available_subexpressions_for_type = GTree::available_subexpressions[eptt];
    const size_t mod_index = index % available_subexpressions_for_type.size();

    return available_subexpressions_for_type[mod_index].evaluate();
}

void GTree::registerLastInsertedNodeAsSubexpression() {
    const size_t last_index = tree_nodes.size() - 1;
    const EncodedPhenotypeType node_type = tree_nodes[last_index]._function.getOutputType();
    GTree::available_subexpressions[node_type].push_back(tree_nodes.size() - 1);
}

std::string GTree::printStaticData() {
    std::stringstream ss;

    ss << "TREE NODES\n"; 

    for (int i = 0; i < GTree::tree_nodes.size(); ++i) {
        ss << "    " <<  i << ": " << tree_nodes[i].toString() << '\n';
    }

    ss << "\nSUBEXPRESSIONS";

    for (auto it = GTree::available_subexpressions.begin(); it != GTree::available_subexpressions.end(); it++) {
        ss << "\n    " << EncodedPhenotypeTypeToString(it -> first) << ":";
        for (auto vit = it -> second.begin(); vit != it -> second.end(); vit++) {
            ss << " " << vit -> getIndex();
        }
    }

    ss << '\n';

    return ss.str();
}

// GTree::GFunction method implementation

std::vector<GTree> GTree::tree_nodes;
std::map<EncodedPhenotypeType, std::vector<GTree::GTreeIndex>> GTree::available_subexpressions;

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
    this -> _build_explicit_form = gf._build_explicit_form;
    this -> _output_type = gf._output_type;
}

GTree::GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _build_explicit_form = init.build_explicit_form;
    this -> _output_type = init.output_type;

    // autoReference functions contain "autoref" in their name
    this -> _is_autoreference  = this -> _name.find("AutoRef") != std::string::npos;
}

void GTree::GFunction::_assert_parameter_format(const std::vector<enc_phen_t>& arg) {
    std::vector<EncodedPhenotypeType> arg_types;
    for_each(arg.begin(), arg.end(), [&](enc_phen_t argument) { arg_types.push_back(argument.getType()); });
    if (!this -> _is_autoreference && (this -> _param_types != arg_types)) {
        throw std::runtime_error(ErrorCodes::BAD_GFUNCTION_PARAMETERS);
    }
}

EncodedPhenotypeType GTree::GFunction::getOutputType() const { return this -> _output_type; }
bool GTree::GFunction::getIsAutoreference() const { return this -> _is_autoreference; }

enc_phen_t GTree::GFunction::evaluate(const std::vector<enc_phen_t>& arg) { 
    this -> _assert_parameter_format(arg);
    return this -> _compute(arg); 
}

GTree::GTreeIndex GTree::GFunction::operator()(std::initializer_list<GTree::GTreeIndex> children) {
    GTree::tree_nodes.push_back(GTree(
        *this,
        children,
        0
    ));

    GTree::registerLastInsertedNodeAsSubexpression();

    return tree_nodes.size() - 1;
}

GTree::GTreeIndex GTree::GFunction::operator()(const std::vector<GTree::GTreeIndex> children) {
    GTree::tree_nodes.push_back(GTree(
        *this,
        children,
        0
    ));

    GTree::registerLastInsertedNodeAsSubexpression();

    return tree_nodes.size() - 1;
}

GTree::GTreeIndex GTree::GFunction::operator()(float x) {
    if (gfunctionAcceptsFloatParameter(*this)) {
        // Only parameter GFunctions like n, f or i are allowed to receive
        // a float as a parameter
        throw std::runtime_error(ErrorCodes::BAD_GFUNCTION_PARAMETERS);
    }

    GTree p_tree = GTree(*this, {}, x);
    
    GTree::tree_nodes.push_back((p_tree));

    GTree::registerLastInsertedNodeAsSubexpression();

    return tree_nodes.size() - 1;
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
std::string GTree::GFunction::buildExplicitForm(std::vector<std::string> v) {
    return this -> _build_explicit_form(v);
}

// GTree method implementation

void GTree::clean() {
    tree_nodes.clear();
    available_subexpressions.clear();
}

GTree::GTree(GTree::GFunction& function, std::vector<GTree::GTreeIndex> children, float leaf_value): _function(function) {
    this -> _children = children;
    this -> _leaf_value = leaf_value;
}

enc_phen_t GTree::evaluate() {
    if (isEncodedPhenotypeTypeAParameterType(this -> _function.getOutputType())) {
        return this -> _function.evaluate({
            EncodedPhenotype({
                .type = leafF,
                .child_type = leafF,
                .children = {},
                .to_string = [&](std::vector<std::string>) { return std::to_string(this -> _leaf_value); },
                .leaf_value = this -> _leaf_value,
            })
        });
    }

    std::vector<enc_phen_t> evaluated_children;
    for_each(this -> _children.begin(), this -> _children.end(), 
        [&](GTree::GTreeIndex child) { 
            evaluated_children.push_back(child.evaluate()); 
        }
    );
    return this -> _function.evaluate(evaluated_children);
}

std::string GTree::toString() {
    if (gfunctionAcceptsFloatParameter(this -> _function)) {
        return this -> _function.buildExplicitForm({Parameter(this -> _leaf_value).toString()});
    }

    std::vector<std::string> string_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree::GTreeIndex index) { 
        string_children.push_back(tree_nodes[index].toString()); 
    });
    
    return this -> _function.buildExplicitForm(string_children);
}

std::function<enc_phen_t(std::vector<enc_phen_t>)> 
    buildParameterComputeFunction(EncodedPhenotypeType parameterType, std::string name, std::function<float(float)> parameter_encoder) {
        if (!isEncodedPhenotypeTypeAParameterType(parameterType)) {
            throw std::runtime_error(ErrorCodes::INVALID_CALL);
        }

        // TO DO: mapper is not correctly initialized
        
        return [=](std::vector<enc_phen_t> params) -> enc_phen_t {
            const float encoded_parameter_value = parameter_encoder(params[0].getLeafValue());
            return EncodedPhenotype({
                .type = parameterType,
                .child_type = leafF,
                .children = params,
                .to_string = [=](std::vector<std::string> children_strings) { return name + "(" + std::to_string(encoded_parameter_value) + ")"; },
                .leaf_value = encoded_parameter_value,
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
    .param_types = { leafF },
    .output_type = noteValueF,
    .compute = buildParameterComputeFunction(noteValueF, "n", [](float v){ return (log(v) + 8 * log(2)) / 10 * log(2); }),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "n(" + children[0] + ")"; 
    }
}),

d({
    .name = "d",
    .param_types = { leafF },
    .output_type = durationF,
    .compute = buildParameterComputeFunction(durationF, "d", [](float p){ return pow(10 * p - 6, 2); }),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "d(" + children[0] + ")"; 
    }
}),

m({
    .name = "m",
    .param_types = { leafF },
    .output_type = midiPitchF,
    .compute = buildParameterComputeFunction(midiPitchF, "m", [](float m){ return (m - 12) / 100; }),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "m(" + children[0] + ")"; 
    }
}),

a({
    .name = "a",
    .param_types = { leafF },
    .output_type = articulationF,
    .compute = buildParameterComputeFunction(articulationF, "a", [](float a){ return pow(a / 3, 1 / E); }),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "a(" + children[0] + ")"; 
    }
}),

i({
    .name = "i",
    .param_types = { leafF },
    .output_type = intensityF,
    .compute = buildParameterComputeFunction(intensityF, "i", [](float i){ return i / 127; }),
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "i(" + children[0] + ")"; 
    }
}),

// q({
//     .name = "q",
//     .param_types = { leafF },
//     .output_type = quantizedF,
//     .compute = buildParameterComputeFunction(quantizedF, "q", [](float f){ return f; }),
//     .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
//         return "i(" + children[0] + ")"; 
//     }
// }),


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
        return "vConcatV(" + join(children, ", ") + ")"; 
    }
}),

eAutoRef({
    .name = "eAutoRef",
    .param_types = { quantizedF },
    .output_type = eventF,
    .compute = [](std::vector<enc_phen_t> params) -> enc_phen_t {
        return GTree::evaluateAutoReference(eventF, (size_t) params[0].getLeafValue());
    },
    .build_explicit_form = [](std::vector<std::string> children) -> std::string { 
        return "vConcatV(" + join(children, ", ") + ")"; 
    }
});

GTree::GFunction e = e_piano;