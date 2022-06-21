#include <algorithm>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "decoded_genotype.hpp"
#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "genomus-core.hpp"
#include "utils.hpp"
#include "testing_utils.hpp"

using namespace std;

GTest DecodedGenotypesTest = GTest("Decoded GenotypesTest")

    .testCase("Functional tree build", [](ostream& os) {

        GTree::GTreeIndex event = e({
            n(1.0),
            m(2.0),
            a(3.0),
            i(1),
        });

        GTree::GTreeIndex tree = vConcatV({vConcatE({event, event}), vConcatE({event, event})});

        os << "Decoded genotype\n";
        os << prettyPrint(tree.toString()) << endl;

        os << "\nEncoded Phenotype\n";
        os << prettyPrint(tree.evaluate().toString()) << endl;

        os << "\nStatic data members\n";
        os << GTree::printStaticData() << endl;

        tree.clean();
    })

    .testCase("Concat functions", [](ostream& os) {
        dec_gen_t event = e_piano({
            n(1.0),
            m(2.0),
            a(3.0),
            i(1),
        });

        dec_gen_t voice = vConcatE({event, event});
        dec_gen_t voice2 = vConcatV({voice, voice});

        os << voice.toString();
        os << voice2.toString();

        dec_gen_t::clean();
    })

    .testCase("AutoReferences", [](ostream& os) {

        GTree::GTreeIndex tree = vConcatV({vConcatE({e({n(1.0), m(2.0), a(3.0), i(1)}), eAutoRef(0)}), vConcatE({eAutoRef(1), eAutoRef(2)})});

        os << "Decoded genotype\n";
        os << prettyPrint(tree.toString()) << endl;

        os << "\nEncoded Phenotype\n";
        os << prettyPrint(tree.evaluate().toString()) << endl;

        os << "\nStatic data members\n";
        os << GTree::printStaticData() << endl;

        tree.clean();

        printOutput();
    });