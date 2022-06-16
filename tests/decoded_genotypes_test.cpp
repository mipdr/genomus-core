#include <algorithm>
#include <ostream>
#include <stdexcept>
#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "genomus-core.hpp"
#include "utils.hpp"
#include "testing_utils.hpp"

using namespace std;

void printOutput() { throw runtime_error("Dummy print error"); }

GTest DecodedGenotypesTest = GTest("Decoded GenotypesTest")
    .testCase("Simpler tree build", [](ostream& os) {
        auto p_tree = GTree(
            p,
            {},
            1.0
        );

        auto tree = e_piano({
            n({&p_tree}),
            m({&p_tree}),
            a({&p_tree}),
            i({&p_tree}),
        });

        os << tree -> toString() << endl;
        os << tree -> evaluate().toString() << endl;

        printOutput();
    });

    // .testCase("GFunction call: eventF", []() {
    //     e({
    //         Parameter(0.1),
    //         Parameter(0.2),
    //         Parameter(0.3)
    //     });
    // })

    // .testCase("GTree declaration and evaluation", [](ostream& os) {
    //     auto param_tree = GTree(
    //         p,
    //         {},
    //         1.0
    //     );

    //     auto event_tree = GTree(
    //         e,
    //         { &param_tree, &param_tree, &param_tree }
    //     );

    //     os << event_tree.evaluate().toString();
    //     os << event_tree.toString();
    // })

    // .testCase("GFunction parameter type list equivalence", []() {
    //     string failure_list = "";

    //     const std::map<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> equivalenceTable({
    //         { {}, {} },
    //         { { ept_event }, { ept_event } },
    //         { { ept_events }, { ept_event } },
    //         { { ept_events }, { ept_event, ept_event } },
    //         { { ept_events, ept_voice }, { ept_event, ept_event, ept_voice } },
    //         { { ept_events, ept_voices }, { ept_event, ept_event, ept_voice, ept_voice } },
    //         {
    //             { ept_parameter, ept_leaf, ept_events, ept_parameter, ept_voices },
    //             { ept_parameter, ept_leaf, ept_event, ept_event, ept_event, ept_event, ept_parameter, ept_voice, ept_voice }
    //         }
    //     });

    //     for_each(equivalenceTable.begin(), equivalenceTable.end(), [&](std::pair<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> entry) {
    //         const bool equivalent = areParameterTypeListsCompatible(entry.first, entry.second);
    //         if (!equivalent) {
    //             throw runtime_error("Expected equivalence");
    //         }
    //     });

    //     const std::map<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> nonEquivalenceTable({
    //         { {}, { ept_event } },
    //         { { ept_voice }, { ept_event } },
    //         { { ept_voices }, { ept_event } },
    //         { { ept_voices }, { ept_voice, ept_event } },
    //         { { ept_events }, { ept_voice, ept_event } },
    //         { { ept_events }, { } },
    //         { { ept_parameter, ept_events }, { ept_parameter } },
    //         { { ept_parameter, ept_events }, { ept_parameter, ept_voice } },
    //     });

    //     for_each(nonEquivalenceTable.begin(), nonEquivalenceTable.end(), [&](std::pair<std::vector<EncodedPhenotypeType>, std::vector<EncodedPhenotypeType>> entry) {
    //         const bool equivalent = areParameterTypeListsCompatible(entry.first, entry.second);
    //         if (equivalent) {
    //             throw runtime_error("Equivalence not expected");
    //         }
    //     });
    // })

    // .testCase("GFunction arbitrary parameters", []() {
    //     auto event = e({
    //         Parameter(1.0),
    //         Parameter(1.0),
    //         Parameter(1.0)
    //     });
    //     v({ event, event, event, event, event });
    // })

    // .testCase("Ejemplo para Pepe", [](ostream& os) {
    //     auto fenotipo = Voice({
    //         Event({
    //             Parameter(1.0), Parameter(1.0), Parameter(1.0)
    //         }),
    //     });

    //     os << fenotipo.toString();

    //     throw runtime_error("dummy");
    // })

    // .testCase("Ejemplo para Pepe 2", [](ostream& os) {
    //     auto fenotipo = v({
    //         e({
    //             Parameter(1.0),
    //             Parameter(1.0),
    //             Parameter(1.0),
    //         })
    //     });

    //     os << fenotipo.toString();

    //     throw runtime_error("dummy");
    // });

    // .testCase("Parameter types", [](ostream& os) {
    //     auto x = e_piano({
    //         n({ Parameter(1.0) }),
    //         m({ Parameter(1.0) }),
    //         a({ Parameter(1.0) }),
    //         i({ Parameter(1.0) }),
    //     });

    //     os << x.toString();
    // })

    // .testCase("Concat functions", [](ostream& os) {
    //     auto event = e_piano({
    //         n({ Parameter(1.0) }),
    //         m({ Parameter(1.0) }),
    //         a({ Parameter(1.0) }),
    //         i({ Parameter(1.0) }),
    //     });

    //     auto voice = vConcatE({event, event});
    //     auto voice2 = vConcatV({voice, voice});

    //     os << voice.toString();
    //     os << voice2.toString();
    // })