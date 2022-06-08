#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "species.hpp"
#include <functional>
#include <stdexcept>
#include <vector>
#include <algorithm>


// GFunction method implementation

GFunction::GFunction(){ 
    this -> _name = "Not initialized decoded_genotype_level_function";
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = vector<EncodedPhenotypeType>();
    this -> _compute = [](vector<enc_phen_t> x) -> enc_phen_t { return Parameter(-1.0); };
    this -> _output_type = ept_event;
}

GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _output_type = init.type;
}

void GFunction::_assert_parameter_format(const vector<enc_phen_t>& arg) {
    vector<EncodedPhenotypeType> arg_types;
    for_each(arg.begin(), arg.end(), [&](enc_phen_t argument) { arg_types.push_back(argument.getType()); });
    if (arg_types != this -> _param_types) {
        throw runtime_error("Bad call to GFunction: bad parameter array.");
    }
}

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
function<string(vector<string>)> GFunction::getBuildExplicitForm() { return this -> _build_explicit_form; }

// GTree method implementation

// GNode::GNode(GFunction* function, vector<GTree*> children) {
//     this -> _function = function;
//     this -> _children = children;
// }

// enc_phen_t GNode::evaluate() {
//     vector<enc_phen_t> evaluated_children;
//     transform(this -> _children.begin(), this -> _children.end(), evaluated_children.begin(), [](GTree* node){ return node -> evaluate(); });
//     return this -> _function -> evaluate(evaluated_children);
// }

// string GNode::toString() {
//     vector<string> string_children;
//     transform(this -> _children.begin(), this -> _children.end(), string_children.begin(), [](GTree* node){ return node -> toString(); });
//     return this -> _function -> getBuildExplicitForm()(string_children);
// }

// GLeaf::GLeaf(ParameterMapper* pm, float value) {
//     this -> _function = pm;
//     this -> _param = value;
// }


// GFunction instances
GFunction eventF;

void initialize_dec_gen_lvl_functions() {

    eventF = GFunction({
        .name = "dec_gen_lvl_expl_function",
        .param_types = { ept_parameter, ept_parameter, ept_parameter },
        .type = ept_event,
        .compute = [](vector<enc_phen_t> params) -> enc_phen_t {
            return Event(params);
        },
        .build_explicit_form = [](vector<string> children) -> string { 
            return string("eventF(") + children[0] + ", " + children[1] + ")"; 
        }
    });
}


