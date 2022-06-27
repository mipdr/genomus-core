#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>

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

        vector<double> expectedNormalizedVector = { 1.000000, 0.236068, 1.000000, 
                                                    0.090170, 0.510000, 0.389195, 
                                                    0.000000, 1.000000, 0.326238, 
                                                    0.530000, 0.000931, 0.000000, 
                                                    1.000000, 0.562306, 0.550000, 
                                                    0.001637, 0.000000, 1.000000, 
                                                    0.180340, 0.560000, 0.000963, 
                                                    0.000000, 0.000000
                                                };

        // printed output for debugging
        os << "Genotype (readable): " << tree.toString() << '\n';
        os << "Genotype (numbers): [" << join(tree.toNormalizedVector()) << "]\n";
        os << "\n\n[" << humanReadableNormalizedVector(tree.toNormalizedVector()) << "]\n";

        if (expectedNormalizedVector != tree.toNormalizedVector()) {
            throw runtime_error("Unexpected normalized vector value.");
        }

        printOutput();
    })

    .testCase("Encoding integers", [](ostream& os) {

        std::map<size_t, double> source_of_truth = {
            { 89, 0.005025 },
            { 34, 0.013156 },
            { 68, 0.026311 },
            { 13, 0.034442 },
            { 47, 0.047597 },
            { 81, 0.060753 },
            { 26, 0.068884 },
            { 60, 0.082039 },
            { 5, 0.09017 },
            { 94, 0.095195 },
            { 39, 0.103326 },
            { 73, 0.116481 },
            { 18, 0.124612 },
            { 52, 0.137767 },
            { 86, 0.150923 },
            { 31, 0.159054 },
            { 65, 0.172209 },
            { 10, 0.18034 },
            { 99, 0.185365 },
            { 44, 0.193496 },
            { 78, 0.206651 },
            { 23, 0.214782 },
            { 57, 0.227937 },
            { 2, 0.236068 },
            { 91, 0.241093 },
        };

        for (auto [integer, normalized] : source_of_truth) {
            if (normalizedToInteger(normalized) != integer) {
                throw runtime_error("Expected integer decoding to work: " + to_string(integer) + " -> " + to_string(normalized));
            }
        }
    });

