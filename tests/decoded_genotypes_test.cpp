#include <iostream>
#include <stdexcept>
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "genomus-core.hpp"
#include "testing_utils.hpp"

using namespace std;

GTest DecodedGenotypesTest = GTest("Decoded GenotypesTest")

    .before([]() {
        initialize_dec_gen_lvl_functions();
    })
    
    .testCase("GFunction call: eventF", [](ostream&) {
        eventF({
            Parameter(0.1),
            Parameter(0.2),
            Parameter(0.3)
        });
    });