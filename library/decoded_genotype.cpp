#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "species.hpp"
#include "utils.hpp"

// GFunction method implementation

GFunction::GFunction(){ 
    this -> _name = "Not initialized decoded_genotype_level_function";
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = vector<EncodedPhenotypeType>();
    this -> _compute = [](vector<enc_phen_t> x) -> enc_phen_t { return Parameter(-1.0); };
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

void GFunction::_assert_parameter_format(const vector<enc_phen_t>& arg) {
    vector<EncodedPhenotypeType> arg_types;
    for_each(arg.begin(), arg.end(), [&](enc_phen_t argument) { arg_types.push_back(argument.getType()); });
    if (arg_types != this -> _param_types) {
        throw runtime_error("Bad call to GFunction: bad parameter array.");
    }
}

EncodedPhenotypeType GFunction::getOutputType() { return this -> _output_type; }

enc_phen_t GFunction::evaluate(const vector<enc_phen_t>& arg) { 
    this -> _assert_parameter_format(arg);
    return this -> _compute(arg); 
}
enc_phen_t GFunction::operator()(const vector<enc_phen_t>& arg) { return this -> evaluate(arg); }

string GFunction::toString() { 
    string ret = "--- GFunction object ---";

    ret += "\n\t_name: " + this -> getName();
    ret += "\n\t_type: " + this -> getTypeString();
    ret += "\n\t_param_types: ";
    for_each(this -> _param_types.begin(), this -> _param_types.end(), [&ret](EncodedPhenotypeType ft){ ret += EncodedPhenotypeTypeToString(ft) + ", "; });
    ret = ret.substr(0, ret.length() - 2) + ";";

    ret += "\n\t_output_type: " + EncodedPhenotypeTypeToString(this -> _output_type) + ";";

    return ret + "\n";
}

vector<EncodedPhenotypeType> GFunction::getParamTypes() { return this -> _param_types; }
// function<string(vector<string>)>& GFunction::getBuildExplicitForm() { return this -> _build_explicit_form; }
string GFunction::buildExplicitForm(vector<string> v) {
    return this -> _build_explicit_form(v);
}


// GTree method implementation

GTree::GTree(GFunction& function, vector<GTree*> children, float leaf_value): _function(function) {
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
                .to_string = [](vector<string>) { return "# to do #"; },
                .leaf_value = this -> _leaf_value,
            })
        });
    }
    // Future work: store evaluation so that each node is only evaluated once.
    vector<enc_phen_t> evaluated_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree* child_ptr) { evaluated_children.push_back(child_ptr -> evaluate()); });
    return this -> _function(evaluated_children);
}

string GTree::toString() {
    if (this -> _function.getOutputType() == ept_parameter) {
        return "parameter(to do)";
    }

    vector<string> string_children;
    for_each(this -> _children.begin(), this -> _children.end(), [&](GTree* child_ptr) { 
        string_children.push_back(child_ptr -> toString()); 
    });

    cout << [](vector<string> children) -> string { 
            return string("eventF(") + join(children, ", ") + ")"; 
        }(string_children) << endl;
    
    return this -> _function.buildExplicitForm(string_children);
}

// GFunction instances
GFunction eventF, paramF, voiceF;

void initialize_dec_gen_lvl_functions() {

    eventF = GFunction({
        .name = "eventF",
        .param_types = { ept_parameter, ept_parameter, ept_parameter },
        .output_type = ept_event,
        .compute = [](vector<enc_phen_t> params) -> enc_phen_t {
            return Event(params);
        },
        .build_explicit_form = [](vector<string> children) -> string { 
            return string("eventF(") + join(children, ", ") + ")"; 
        }
    });

    paramF = GFunction({
        .name = "paramF",
        .param_types = { ept_leaf },
        .output_type = ept_parameter,
        .compute = [](vector<enc_phen_t> params) -> enc_phen_t {
            return Parameter(params);
        },
        .build_explicit_form = [](vector<string> children) -> string { 
            return "paramF()"; 
        }
    });

    voiceF = GFunction({
        .name = "voiceF",
        .param_types = { ept_event, ept_event },
        .output_type = ept_voice,
        .compute = [](vector<enc_phen_t> params) -> enc_phen_t {
            return Voice(params);
        },
        .build_explicit_form = [](vector<string> children) -> string { 
            return "voiceF(" + join(children, ", ") + ")"; 
        }
    });
}
