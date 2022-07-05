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
#include "species.hpp"
#include "utils.hpp"
#include "errorCodes.hpp"

// utils

bool isEncodedPhenotypeTypeAParameterType(EncodedPhenotypeType eptt) {
    static const std::vector<EncodedPhenotypeType> parameterTypes({
        noteValueF,
        durationF,
        midiPitchF,
        frequencyF,
        articulationF,
        intensityF,
        leafF,
        paramF,
        quantizedF,
        goldenintegerF,
    });
    return includes(parameterTypes, eptt);
}

bool isEncodedPhenotypeTypeAListType(EncodedPhenotypeType eptt) {
    static const std::vector<EncodedPhenotypeType> parameterTypes({
        lnoteValueF,
        ldurationF,
        lmidiPitchF,
        lfrequencyF,
        larticulationF,
        lintensityF,
        listF,
        lquantizedF,
        lgoldenintegerF,
    });
    return includes(parameterTypes, eptt);
}

bool gfunctionAcceptsNumericParameter(const GTree::GFunction& gf) {
    return (gf.getIsAutoreference() || isEncodedPhenotypeTypeAParameterType(gf.getOutputType())) && !gf.getIsRandom();
}

double leafTypeToNormalizedValue(EncodedPhenotypeType eptt) {
    if (!isEncodedPhenotypeTypeAParameterType(eptt)) 
        throw std::runtime_error(ErrorCodes::PARAMETER_IS_NOT_A_LEAF);

    // Table filled according to specifications: 
    // https://github.com/lopezmontes/GenoMus/blob/master/specifications/GenoMus_specifications.md#encoding-decoding-genotypes
    static const std::map<EncodedPhenotypeType, double> leafTypeEncoding = {
        { leafF, 0.50 },
        { noteValueF, 0.51 },
        { durationF, 0.52 },
        { midiPitchF, 0.53 },
        { frequencyF, 0.54 },
        { articulationF, 0.55 },
        { intensityF, 0.56 },
        { goldenintegerF, 0.57 },
        { quantizedF, 0.58 },
    };

    return findWithDefault(leafTypeEncoding, eptt, 0.5);
}

EncodedPhenotypeType listToParameterType(EncodedPhenotypeType listType) {
    if (!isEncodedPhenotypeTypeAListType(listType)) 
        throw std::runtime_error(ErrorCodes::PARAMETER_IS_NOT_A_LIST);

    static const std::map<EncodedPhenotypeType, EncodedPhenotypeType> listToParam {
        { listF, leafF },
        { lnoteValueF, noteValueF },
        { ldurationF, durationF },
        { lmidiPitchF, midiPitchF },
        { lfrequencyF, frequencyF },
        { larticulationF, articulationF },
        { lintensityF, intensityF },
        { lgoldenintegerF, goldenintegerF },
        { lquantizedF, quantizedF },
    };

    return findWithDefault(listToParam, listType, leafF);
}

// GTree::GTreeIndex method implementation

GTree::GTreeIndex::GTreeIndex(size_t i) { this -> _index = i; }
enc_phen_t GTree::GTreeIndex::evaluate(){ return tree_nodes[this -> _index].evaluate(); }
std::string GTree::GTreeIndex::toString() const { 
    return tree_nodes[this -> _index].toString(); 
}
GTree::GTreeIndex::operator size_t() const { return this -> _index; }
GTree::GTreeIndex::operator std::string() const { return this -> toString(); }
double GTree::GTreeIndex::getLeafValue() const {
    return tree_nodes[this -> _index]._leaf_value;
}
size_t GTree::GTreeIndex::getIndex() const { return this -> _index; }

void GTree::GTreeIndex::clean() {
    GTree::clean();
}

std::vector<double> GTree::GTreeIndex::toNormalizedVector() {
    return tree_nodes[this -> _index].toNormalizedVector();
}

EncodedPhenotype GTree::evaluateAutoreference(EncodedPhenotypeType eptt, size_t index, size_t depth_first_index) {
    std::vector<GTree::GTreeIndex> available_subexpressions_for_type = GTree::available_subexpressions[eptt];

    if (available_subexpressions_for_type.size() == 0 || depth_first_index == 0) {
        throw std::runtime_error(ErrorCodes::BAD_AUTOREFERENCE);
    }

    // i is the position of the autoreference in the available subexpressions ofr type
    size_t i;
    for (i = 0; i < available_subexpressions_for_type.size(); ++i) {
        if (available_subexpressions_for_type[i] >= depth_first_index) {
            break;
        }
    }

    if (i == 0) {
        throw std::runtime_error(ErrorCodes::BAD_AUTOREFERENCE);
    }

    size_t mod_index = index % i;
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
        ss << "\n    " << encodedPhenotypeTypeToString(it -> first) << ":";
        for (auto vit = it -> second.begin(); vit != it -> second.end(); vit++) {
            ss << " " << vit -> getIndex();
        }
    }

    ss << '\n';

    return ss.str();
}

std::string unalias_name(std::string name) {
    static std::string unaliased_name;
    unaliased_name = name_aliases.find(name) == name_aliases.end() ? name : name_aliases[name];
    return unaliased_name;
}

// GTree::GFunction method implementation

std::vector<GTree> GTree::tree_nodes;
RandomGenerator GTree::RNG;
std::map<EncodedPhenotypeType, std::vector<GTree::GTreeIndex>> GTree::available_subexpressions;

std::string GTree::GFunction::getName() { return this -> _name; };

GTree::GFunction::GFunction(){ 
    this -> _name = "Not initialized decoded_genotype_level_function";
    this -> _index = 0;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = std::vector<EncodedPhenotypeType>();
    this -> _compute = [](std::vector<enc_phen_t> x) -> enc_phen_t { return Parameter(-1.0); };
    this -> _output_type = leafF;
    this -> _default_function_for_type = false;
    this -> _is_Autoreference = false;
    this -> _is_random = false;
}

GTree::GFunction::GFunction(const GTree::GFunction& gf) {
    this -> _name = gf._name;
    this -> _index = gf._index;
    this -> _type = gf._type;
    this -> _param_types = gf._param_types;
    this -> _compute = gf._compute;
    this -> _output_type = gf._output_type;
    this -> _is_Autoreference = gf._is_Autoreference;
    this -> _default_function_for_type = gf._default_function_for_type;
    this -> _is_Autoreference = gf._is_Autoreference;
    this -> _is_random = gf._is_random;
}

GTree::GFunction::GFunction(const GTree::GFunction& gf, std::string name) {
    this -> _name = name != "" ? name : gf._name;
    this -> _type = gf._type;
    this -> _index = gf._index;
    this -> _param_types = gf._param_types;
    this -> _compute = gf._compute;
    this -> _output_type = gf._output_type;
    this -> _is_Autoreference = gf._is_Autoreference;
    this -> _default_function_for_type = false;
    this -> _is_random = gf._is_random;
}

GTree::GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _index = init.index;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _output_type = init.output_type;
    this -> _is_Autoreference  = init.is_Autoreference;
    this -> _default_function_for_type = init.default_function_for_type;

    this -> _is_random = init.is_random;
}

GTree::GFunction GTree::GFunction::alias(std::string alias_name) {
    if (name_aliases.find(this -> _name) != name_aliases.end()) {
        throw std::runtime_error(ErrorCodes::ALIASING_AN_ALIAS_IS_NOT_SUPPORTED);
    }

    name_aliases[alias_name] = this -> _name;
    return GTree::GFunction({
        .name = alias_name,
        .index = this -> _index,
        .param_types = this -> _param_types,
        .output_type = this -> _output_type,
        .compute = [&](std::vector<enc_phen_t> params) -> enc_phen_t {
            return this -> _compute(params);
        },
    });
}

void GTree::GFunction::_assert_parameter_format(const std::vector<enc_phen_t>& arg) const {
    std::vector<EncodedPhenotypeType> arg_types;
    for_each(arg.begin(), arg.end(), [&](enc_phen_t argument) { arg_types.push_back(argument.getType()); });
    if (!this -> _is_Autoreference && (this -> _param_types != arg_types)) {
        throw std::runtime_error(ErrorCodes::BAD_GFUNCTION_PARAMETERS + ": " + this -> _name);
    }
}

EncodedPhenotypeType GTree::GFunction::getOutputType() const { return this -> _output_type; }
bool GTree::GFunction::getIsAutoreference() const { return this -> _is_Autoreference; }
bool GTree::GFunction::getIsDefaultForType() const { return this -> _default_function_for_type; };
bool GTree::GFunction::getIsRandom() const { return this -> _is_random; };

enc_phen_t GTree::GFunction::evaluate(const std::vector<enc_phen_t>& arg) const { 
    // this -> _assert_parameter_format(arg);
    return this -> _compute(arg); 
}

GTree::GTreeIndex GTree::GFunction::operator()(std::initializer_list<GTree::GTreeIndex> children) {
    GTree::tree_nodes.push_back(GTree(
        *this,
        children,
        0,
        tree_nodes.size()
    ));

    GTree::registerLastInsertedNodeAsSubexpression();

    return tree_nodes.size() - 1;
}

GTree::GTreeIndex GTree::GFunction::operator()(const std::vector<GTree::GTreeIndex> children) {
    GTree::tree_nodes.push_back(GTree(
        *this,
        children,
        0,
        tree_nodes.size()
    ));

    GTree::registerLastInsertedNodeAsSubexpression();

    return tree_nodes.size() - 1;
}

GTree::GTreeIndex GTree::GFunction::operator()(double x) {
    if (!gfunctionAcceptsNumericParameter(*this)) {
        // Only parameter functions and Autoreferences are allowed to receive a numeric parameter
        throw std::runtime_error(ErrorCodes::BAD_GFUNCTION_PARAMETERS + ": " + this -> _name + " does not accept a double parameter.");
    }
    
    if (this -> _is_Autoreference) {
        if (x > tree_nodes.size() - 1) {
            throw std::runtime_error(ErrorCodes::BAD_AUTOREFERENCE_INDEX);
        }
    }

    GTree p_tree = GTree(*this, {}, x, tree_nodes.size());
    
    GTree::tree_nodes.push_back((p_tree));

    GTree::registerLastInsertedNodeAsSubexpression();

    return tree_nodes.size() - 1;
}

std::string GTree::GFunction::toString() { 
    std::string ret = "--- GTree::GFunction object ---";

    ret += "\n\t_name: " + this -> getName();
    // ret += "\n\t_type: " + EncodedPhenotypeTypeToString(this);
    ret += "\n\t_param_types: ";
    for_each(this -> _param_types.begin(), this -> _param_types.end(), [&ret](EncodedPhenotypeType ft){ ret += encodedPhenotypeTypeToString(ft) + ", "; });
    ret = ret.substr(0, ret.length() - 2) + ";";

    ret += "\n\t_output_type: " + encodedPhenotypeTypeToString(this -> _output_type) + ";";

    return ret + "\n";
}

std::vector<EncodedPhenotypeType> GTree::GFunction::getParamTypes() const { return this -> _param_types; }
size_t GTree::GFunction::getIndex() const { return this -> _index; }
std::string GTree::GFunction::buildExplicitForm(std::vector<std::string> v) {
    return unalias_name(this -> _name) + "(" + join(v) + ")";
}

// GTree method implementation

void GTree::clean() {
    tree_nodes.clear();
    available_subexpressions.clear();
}

GTree::GTree(GTree::GFunction& function, std::vector<GTree::GTreeIndex> children, double leaf_value, size_t depth_first_index): _function(function) {
    this -> _children = children;
    this -> _leaf_value = leaf_value;
    this -> _depth_first_index = depth_first_index;
}

enc_phen_t GTree::evaluate() {
    if (gfunctionAcceptsNumericParameter(this -> _function)) {

        if (this -> _function.getIsAutoreference()) {
            return GTree::evaluateAutoreference(this -> _function.getOutputType(), (size_t) this -> _leaf_value, this -> _depth_first_index);
        }
        return this -> _function.evaluate({
            EncodedPhenotype({
                .type = leafF,
                .child_type = leafF,
                .children = std::vector<EncodedPhenotype>(),
                .to_string = [&](std::vector<std::string>) { return std::to_string(this -> _leaf_value); },
                .leaf_value = this -> _leaf_value,
            })
        });
    } else if(this -> _function.getIsRandom()) {
        if (this -> _leaf_value == 0) {
            auto result = this -> _function.evaluate({});
            this -> _leaf_value = result.getLeafValue();
            return result;
        } else {
            return this -> _function.evaluate({enc_phen_t({
                .type = this -> _function.getOutputType(),
                .child_type = leafF,
                .children = {},
                .to_string = [this](std::vector<std::string> children_strings) { return this -> _function.getName() + "(" + std::to_string(this -> _leaf_value) + ")"; },
                .leaf_value = this -> _leaf_value,
            })});
        }
    }

    std::vector<enc_phen_t> evaluated_children;
    for_each(this -> _children.begin(), this -> _children.end(), 
        [&](GTree::GTreeIndex child) { 
            evaluated_children.push_back(child.evaluate()); 
        }
    );

    return this -> _function.evaluate(evaluated_children);
}

std::vector<double> GTree::toNormalizedVector() {
    std::vector<double> result = {1};
    double encoded_leaf;

    double encoded_index = function_name_to_index[unalias_name(this -> _function.getName())];
    result.push_back(encoded_index);

    if (isEncodedPhenotypeTypeAParameterType(this -> _function.getOutputType())) {
        result.push_back(leafTypeToNormalizedValue(this -> _function.getOutputType()));
        double encoded_leaf;

        if (this -> _function.getIsRandom()) {
            encoded_leaf = (this -> _leaf_value == 0) ? this -> evaluate().getLeafValue() : this -> _leaf_value;
        } else {
            encoded_leaf = this -> evaluate().getLeafValue();
        }

        result.push_back(encoded_leaf);
    } else if (isEncodedPhenotypeTypeAListType(this -> _function.getOutputType())) {
        // result.push_back(integerToNormalized(this -> _children.size()));
        const double leafTypeMarker = leafTypeToNormalizedValue(listToParameterType(this -> _function.getOutputType()));

        for (auto&& child: this -> _children) {
            result.push_back(leafTypeMarker);
            result.push_back(encodeParameter(
                listToParameterType(this -> _function.getOutputType()), 
                child.getLeafValue())
            );
        }
    } else if (this -> _function.getIsAutoreference()) {

    } else {
        std::vector<double> evaluated_children;
        for_each(this -> _children.begin(), this -> _children.end(), 
            [&](GTree::GTreeIndex child) { 
                auto&& aux = child.toNormalizedVector();
                evaluated_children.insert(evaluated_children.end(), aux.begin(), aux.end()); 
            }
        );
        result.insert(result.end(), evaluated_children.begin(), evaluated_children.end());
    }
    
    result.push_back(0);

    return result;
}

std::string GTree::toString() {
    if (gfunctionAcceptsNumericParameter(this -> _function)) {
        return this -> _function.buildExplicitForm({Parameter(this -> _leaf_value).toString()});
    }

    std::vector<std::string> string_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree::GTreeIndex index) { 
        string_children.push_back(tree_nodes[index].toString()); 
    });
    
    return this -> _function.buildExplicitForm(string_children);
}


std::string humanReadableNormalizedVector(std::vector<double> v) {
    std::stringstream ss;

    for_each(v.begin(), v.end(), [&](double d) {
        ss << d;
        ss << (available_functions.find(d) == available_functions.end() ? "" : "(" + available_functions[d].getName() + ")");
        ss << ", ";
    });

    return ss.str();
}