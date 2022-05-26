#include "decoded_genotype.hpp"
#include <vector>

GFunction::GFunction(GFunctionInitializer init) {
    this -> _param_types = init.param_types;
    this -> _compute = init.compute;
    this -> _output_type = init.type;
}

enc_phen_t GFunction::evaluate(vector<GFunction> arg) { return this -> _compute(arg); }
string GFunction::toString() { return "not implemented"; }


GFunction dec_gen_lvl_expl_function;

void initialize_dec_gen_lvl_functions() {
    dec_gen_lvl_expl_function = GFunction({
        .param_types = {leafF, leafF},
        .type = listF,
        .compute = [](vector<GFunction> a){ return a[0].toString() + "////" + a[1].toString(); }
    });
}


