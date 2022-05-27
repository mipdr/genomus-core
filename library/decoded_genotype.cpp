#include "decoded_genotype.hpp"
#include <functional>
#include <stdexcept>
#include <vector>
#include <algorithm>

// FunctionType -> std::string

string functionTypeToString(FunctionType ft) {
    switch (ft) {
        case leafF:
            return "leafF";
        case listF:
            return "listF";
        default:
            return "invalid function type";
    }
}

// GFunction method implementation

GFunction::GFunction(){ 
    this -> _name = "Not initialized decoded_genotype_level_function";
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = vector<FunctionType>();
    this -> _compute = [](vector<GFunction> x) -> enc_phen_t { throw runtime_error("Not Initialized"); };
    this -> _output_type = defaultF;
}

GFunction::GFunction(GFunctionInitializer init) {
    this -> _name = init.name;
    this -> _type = decoded_genotype_level_function;
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _output_type = init.type;
}

enc_phen_t GFunction::evaluate(vector<GFunction> arg) { return this -> _compute(arg); }

string GFunction::toString() { 
    string ret = "--- GFunction object ---";

    ret += "\n\t_name: " + this -> getName();
    ret += "\n\t_type: " + this -> getTypeString();
    ret += "\n\t_param_types: ";
    for_each(this -> _param_types.begin(), this -> _param_types.end(), [&ret](FunctionType ft){ ret += functionTypeToString(ft) + ", "; });
    ret = ret.substr(0, ret.length() - 2) + ";";

    ret += "\n\t_output_type: " + functionTypeToString(this -> _output_type) + ";";

    return ret + "\n";
}

// GFunction instances
GFunction dec_gen_lvl_expl_function;

void initialize_dec_gen_lvl_functions() {
    dec_gen_lvl_expl_function = GFunction({
        .name = "dec_gen_lvl_expl_function",
        .param_types = {leafF, leafF},
        .type = listF,
        .compute = [](vector<GFunction> a){ return a[0].toString() + "////" + a[1].toString(); }
    });
}


