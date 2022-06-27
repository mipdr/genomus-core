#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <bits/stdc++.h>
#include <string>

#include "genomus-core.hpp"
#include "testing_utils.hpp"

using namespace std;

static unsigned int test_case_counter = 1;
static bool verbose = true;

GTest EncodedPhenotypesTest = GTest("Encoded Phenotypes Test")

    .before([]() { init_available_functions(); })
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

        printOutput();
    });

