#include "encoded_phenotype.hpp"
#include "genomus-core.hpp"
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <bits/stdc++.h>
#include "species.hpp"
#include "testing_utils.hpp"

using namespace std;

static unsigned int test_case_counter = 1;
static bool verbose = true;

GTest EncodedPhenotypesTest = GTest("Encoded Phenotypes Test")

    .testCase("Parameter declaration", [](ostream& os){
        os << Parameter(1.0).toString() << endl;
    })

    .testCase("Event declaration", [](ostream& os) {
        auto e = Event({
            Parameter(1.0),
            Parameter(0.5),
            Parameter(0.3)
        });

        os << e.toString() << endl;
        flush(cout);
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

    .testCase("Voice declaration", [](ostream& os) {
        auto v = Voice({
            Event({Parameter(1.0), Parameter(2.0), Parameter(3.0)}),
            Event({Parameter(2.0), Parameter(2.0), Parameter(3.0)}),
            Event({Parameter(3.0), Parameter(2.0), Parameter(3.0)}),
            Event({Parameter(4.0), Parameter(2.0), Parameter(3.0)}),
        });
        os << v.toString() << endl;
    });

