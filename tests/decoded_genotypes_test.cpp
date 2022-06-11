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
    
    .testCase("GFunction call: eventF", []() {
        eventF({
            Parameter(0.1),
            Parameter(0.2),
            Parameter(0.3)
        });
    })

    .testCase("GTree declaration and evaluation", [](ostream& os) {
        auto param_tree = GTree(
            paramF,
            {},
            1.0
        );

        auto event_tree = GTree(
            eventF,
            { &param_tree, &param_tree, &param_tree }
        );

        os << event_tree.evaluate().toString();
        os << event_tree.toString();

        throw runtime_error("dummy error");
    });