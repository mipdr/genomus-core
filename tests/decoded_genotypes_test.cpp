#include <iostream>
#include "decoded_genotype.hpp"
#include "features.hpp"
#include "genomus-core.hpp"
#include "tests.hpp"

using namespace std;

void DecodedGenotypesTest::run() {
    initialize_dec_gen_lvl_functions();

    cout << dec_gen_lvl_expl_function.toString() << endl;

    // cout << featureTypeToString(parameter_mapper) << endl;
    // cout << featureTypeToString(decoded_genotype_level_function) << endl;
}
