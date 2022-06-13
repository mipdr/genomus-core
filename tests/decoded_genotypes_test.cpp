#include <algorithm>
#include <stdexcept>
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "genomus-core.hpp"
#include "utils.hpp"
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
    })

    .testCase("GFunction parameter type list equivalence", []() {
        string failure_list = "";

        const std::map<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> equivalenceTable({
            { {}, {} },
            { { ept_event }, { ept_event } },
            { { ept_events }, { ept_event } },
            { { ept_events }, { ept_event, ept_event } },
            { { ept_events, ept_voice }, { ept_event, ept_event, ept_voice } },
            { { ept_events, ept_voices }, { ept_event, ept_event, ept_voice, ept_voice } },
            {
                { ept_parameter, ept_leaf, ept_events, ept_parameter, ept_voices },
                { ept_parameter, ept_leaf, ept_event, ept_event, ept_event, ept_event, ept_parameter, ept_voice, ept_voice }
            }
        });

        for_each(equivalenceTable.begin(), equivalenceTable.end(), [&](std::pair<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> entry) {
            const bool equivalent = areParameterTypeListsCompatible(entry.first, entry.second);
            if (!equivalent) {
                throw runtime_error("Expected equivalence");
            }
        });

        const std::map<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> nonEquivalenceTable({
            { {}, { ept_event } },
            { { ept_voice }, { ept_event } },
            { { ept_voices }, { ept_event } },
            { { ept_voices }, { ept_voice, ept_event } },
            { { ept_events }, { ept_voice, ept_event } },
            { { ept_events }, { } },
            { { ept_parameter, ept_events }, { ept_parameter } },
            { { ept_parameter, ept_events }, { ept_parameter, ept_voice } },
        });

        for_each(nonEquivalenceTable.begin(), nonEquivalenceTable.end(), [&](std::pair<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> entry) {
            const bool equivalent = areParameterTypeListsCompatible(entry.first, entry.second);
            if (equivalent) {
                throw runtime_error("Equivalence not expected");
            }
        });
    })

    .testCase("GFunction arbitrary parameters", []() {
        auto e = eventF({
            Parameter(1.0),
            Parameter(1.0),
            Parameter(1.0)
        });
        voiceF({ e, e, e, e, e, e });
    });