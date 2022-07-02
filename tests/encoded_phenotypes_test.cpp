#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "decoded_genotype.hpp"
#include "genomus-core.hpp"
#include "testing_utils.hpp"

using namespace std;

static unsigned int test_case_counter = 1;
static bool verbose = true;

GTest EncodedPhenotypesTest = GTest("Encoded Phenotypes Test")

    .before([]() { init_genomus(); })
    .after([]() { GTree::clean(); })

    .testCase("Parameter declaration", [](ostream& os){
        os << Parameter(1.0).toString() << endl;
    })

    .testCase("Bad Event declaration", [](ostream& os) {
        try {
            auto e2 = Event({
                Event({}),
                Parameter(3.0)
            });

        } catch (runtime_error e) {
            return;
        }
        throw runtime_error("Expected bad Event initialization to throw an error.");
    })

    .testCase("Encoded phenotype to normalized vector", [](ostream& os) {
        auto ept = parseString("s(v(e_piano(n(0.100000), m(0.200000), a(0.300000), i(0.400000))))").evaluate();

        os << ept.toString() << "\n";
        os << to_string(ept.toNormalizedVector()) << "\n";
    })

    .testCase("List types", [](ostream& os) {
        GTree::clean();
        auto tree = vMotif({
            ln({p(0.1), p(0.2)}),
            lm({p(0.1), p(0.2)}),
            la({p(0.1), p(0.2)}),
            li({p(0.1), p(0.2)})
        });

        auto ev = tree.evaluate();
        auto s = ev.toString();

        os << s << endl;

        printOutput();
    });

