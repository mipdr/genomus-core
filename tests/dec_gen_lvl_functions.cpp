#include <iostream>
#include "features.hpp"
#include "genomus-core.hpp"

using namespace std;

int main() {
    initialize_dec_gen_lvl_functions();

    cout << dec_gen_lvl_expl_function.toString() << endl;

    // cout << featureTypeToString(parameter_mapper) << endl;
    // cout << featureTypeToString(decoded_genotype_level_function) << endl;
}