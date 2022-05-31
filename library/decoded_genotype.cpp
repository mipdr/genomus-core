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
    this -> _compute = [](vector<enc_phen_t> x) -> enc_phen_t { return Parameter({.parameter_type = duration, .value = 1.0}); };
    this -> _output_type = ept_event;
}

GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _output_type = init.type;
}

enc_phen_t GFunction::evaluate(vector<enc_phen_t> arg) { return this -> _compute(arg); }

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


enc_phen_t GNode::evaluate() {
    vector<enc_phen_t> evaluated_children;
    transform(this -> _children.begin(), this -> _children.end(), evaluated_children.begin(), [](GTree* node){ return node -> evaluate(); });
    return this -> _function -> evaluate(evaluated_children);
}

string GNode::toString() {
    vector<string> string_children;
    transform(this -> _children.begin(), this -> _children.end(), string_children.begin(), [](GTree* node){ return node -> toString(); });
    return this -> _function -> getBuildExplicitForm()(string_children);
}


// GFunction instances
GFunction dec_gen_lvl_expl_function,
    event_identity;

void initialize_dec_gen_lvl_functions() {
    dec_gen_lvl_expl_function = GFunction({
        .name = "dec_gen_lvl_expl_function",
        .param_types = { ept_parameter, ept_parameter},
        .type = ept_event,
        .compute = [](vector<enc_phen_t> a) -> enc_phen_t { 
            return Event({.parameters = {}}, CURRENT_SPECIES); 
        },
        .build_explicit_form = [](vector<string> children) -> string { 
            return string("dec_gen_lvl_expl_function(") + children[0] + ", " + children[1] + ")"; 
        }
    });

    event_identity = GFunction({
        .name = "dec_gen_lvl_expl_function",
        .param_types = { ept_parameter, ept_parameter},
        .type = ept_event,
        .compute = [](vector<enc_phen_t> a) -> enc_phen_t { 
            return Event({.parameters = {}}, CURRENT_SPECIES); 
        },
        .build_explicit_form = [](vector<string> children) -> string { 
            return string("dec_gen_lvl_expl_function(") + children[0] + ", " + children[1] + ")"; 
        }
    });
}


