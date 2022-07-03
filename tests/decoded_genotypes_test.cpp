#include <algorithm>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "genomus-core.hpp"
#include "testing_utils.hpp"

using namespace std;

GTest DecodedGenotypesTest = GTest("Decoded GenotypesTest")

    .after([]() { GTree::clean(); })

    .testCase("Functional tree build", [](ostream& os) {

        GTree::GTreeIndex event = e_piano({
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

        GTree::GTreeIndex tree = vConcatV({vConcatE({e_piano({n(1.0), m(2.0), a(3.0), i(1)}), eAutoRef(0)}), vConcatE({eAutoRef(1), eAutoRef(2)})});

        os << "Decoded genotype\n";
        os << prettyPrint(tree.toString()) << endl;

        os << "\nEncoded Phenotype\n";
        os << prettyPrint(tree.evaluate().toString()) << endl;

        os << "\nStatic data members\n";
        os << GTree::printStaticData() << endl;

        tree.clean();
    })

    .testCase("Lists", [](ostream& os) {
        auto tree = vMotif({
            ln({p(0.1), p(0.2)}),
            lm({p(0.1), p(0.2)}),
            la({p(0.1), p(0.2)}),
            li({p(0.1), p(0.2)})
        });
    });