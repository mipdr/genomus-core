#include "encoded_phenotype.hpp"
#include "genomus-core.hpp"
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <bits/stdc++.h>
#include "species.hpp"
#include "tests.hpp"

using namespace std;

static unsigned int test_case_counter = 1;
static bool verbose = true;

void testCase(string title, function<void(ostream& os)> test) {
    cout << "----- TEST CASE " << test_case_counter << ": " << title << " -> ";
    test_case_counter++;

    stringstream ss;
    try {
        test(ss);
    } catch (runtime_error e) {
        cout << "FAILED" << endl;
        if (ss.str().length()) {
            cout << "\tTest output:" << endl;
            cout << regex_replace(ss.str(), regex("\n"), "\n\t") << endl;
        }
        cout << "\n" << e.what() << "\n";
        return;
    }
    cout << "OK" << endl; 
    if (verbose) {
        cout << "\n" << ss.str() << endl;
    }
}

void EncodedPhenotypesTest::run() {
    cout << "ENCODED PHENOTYPES TESTS ..." << endl;

    testCase("Parameter declaration", [](ostream& os){
        auto p = Parameter(1.0);
        os << p.toString() << endl;
    });

    testCase("Event declaration", [](ostream& os) {
        auto e = Event({
            Parameter(1.0),
            Parameter(0.5),
            Parameter(0.3)
        });

        os << e.toString() << endl;
        flush(cout);
    });

    testCase("Bad Event declaration", [](ostream& os) {
        auto e2 = Event({
            Event({}),
            Parameter(3.0)
        });
    });

    testCase("Voice declaration", [](ostream& os) {
        auto v = Voice({
            Event({Parameter(1.0), Parameter(2.0), Parameter(3.0)}),
            Event({Parameter(2.0), Parameter(2.0), Parameter(3.0)}),
            Event({Parameter(3.0), Parameter(2.0), Parameter(3.0)}),
            Event({Parameter(4.0), Parameter(2.0), Parameter(3.0)}),
        });
        os << v.toString() << endl;
    });

}
