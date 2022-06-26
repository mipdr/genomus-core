#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <bits/stdc++.h>

#include "decoded_genotype.hpp"
#include "genomus-core.hpp"
#include "testing_utils.hpp"
#include "utils.hpp"

using namespace std;

static unsigned int test_case_counter = 1;
static bool verbose = true;

string humanReadableNormalizedVector(vector<double>&& v) {
    stringstream ss;

    for_each(v.begin(), v.end(), [&](double d) {
        ss << d;
        ss << (available_functions.find(d) == available_functions.end() ? "" : "(" + available_functions[d].getName() + ")");
        ss << ", ";
    });

    return ss.str();
}

GTest EncodedGenotypesTest = GTest("Encoded Genotypes Test")

    .before([]() { init_available_functions(); })
    .after([]() { GTree::clean(); })

    .testCase("Decoded genotype to encoded genotype", [](ostream& os){
        auto tree = vConcatV({vConcatE({e_piano({n(0.1), m(0.1), a(0.1), i(0.1)}), eAutoRef(0.1)}), vConcatE({eAutoRef(0.1), eAutoRef(0.1)})});

        tree = e({n(0.1), m(0.2), a(0.3), i(0.4)});

        os << "Genotype (readable): " << tree.toString() << '\n';
        os << "Genotype (numbers): [" << join(tree.toNormalizedVector()) << "]\n";

        os << "\n\n[" << humanReadableNormalizedVector(tree.toNormalizedVector()) << "]\n";

        printOutput();
    });

