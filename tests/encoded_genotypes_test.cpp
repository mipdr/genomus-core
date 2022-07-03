#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>

#include "encoded_genotype.hpp"
#include "genomus-core.hpp"
#include "parser.hpp"
#include "testing_utils.hpp"

using namespace std;

static unsigned int test_case_counter = 1;
static bool verbose = true;

GTest EncodedGenotypesTest = GTest("Encoded Genotypes Test")

    .before([]() { init_genomus(); })
    .after([]() { GTree::clean(); })

    // .testCase("Decoded genotype to encoded genotype", [](ostream& os){
    //     auto tree = vConcatV({vConcatE({e_piano({n(0.1), m(0.1), a(0.1), i(0.1)}), eAutoRef(0.1)}), vConcatE({eAutoRef(0.1), eAutoRef(0.1)})});

    //     tree = e({n(0.1), m(0.2), a(0.3), i(0.4)});

    //     vector<double> expectedNormalizedVector = { 1.000000, 0.236068, 1.000000, 
    //                                                 0.090170, 0.510000, 0.389195, 
    //                                                 0.000000, 1.000000, 0.326238, 
    //                                                 0.530000, 0.000931, 0.000000, 
    //                                                 1.000000, 0.562306, 0.550000, 
    //                                                 0.001637, 0.000000, 1.000000, 
    //                                                 0.180340, 0.560000, 0.000963, 
    //                                                 0.000000, 0.000000
    //                                             };

    //     // printed output for debugging
    //     os << "Genotype (readable): " << tree.toString() << '\n';
    //     os << "Genotype (numbers): [" << join(tree.toNormalizedVector()) << "]\n";
    //     os << "\n\n[" << humanReadableNormalizedVector(tree.toNormalizedVector()) << "]\n";

    //     if (expectedNormalizedVector != tree.toNormalizedVector()) {
    //         throw runtime_error("Unexpected normalized vector value. Expected value: " + to_string(expectedNormalizedVector));
    //     }
    // })

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
    })

    .testCase("Vector aproximation", []() {
        vector<double> v = {0, 1, 2, 3, 4, 5};

        static const map<double, double> source_of_truth = {
            {-5, 0},
            {0.4, 0},
            {1, 1},
            {3.6, 4},
            {10000, 5},
        };

        for (auto [value, normalized] : source_of_truth) {
            if (getClosestValue(v, value) != normalized) {
                throw runtime_error("Expected: getClosestValue(" + to_string(value) + ") -> " + to_string(normalized) + " but obtained " + to_string(getClosestValue(v, value)));
            }
        }
    })

    .testCase("Germinal vector to genotype", [](ostream& os){
        auto tree =  s({v({e_piano({n(0.1), m(0.1), a(0.1), i(0.1)})})});
        auto v = tree.toNormalizedVector();

        std::vector<double> result;
        normalizeVector(v, result);

        os << "Normalized vector: ";
        os << to_string(result) << endl;
        os << "Original vector: " << to_string(v) << endl;
    })

    .testCase("Normalize random vector", [](ostream& os){
        std::vector<double> v = newGerminalVector();
        std::vector<double> normalized;
        std::vector<double> second_normalized;

        normalizeVector(v, normalized);
        normalizeVector(normalized, second_normalized);

        os << "Original vector: " << humanReadableNormalizedVector(v) << endl << endl << endl;
        // os << "Normalized vector: " << humanReadableNormalizedVector(normalized) << endl << endl << endl;
        // os << "Two times normalized vector: " << humanReadableNormalizedVector(second_normalized) << endl << endl;


        for (size_t i = 0; i < std::max(normalized.size(), second_normalized.size()); ++i) {
            os << (i < normalized.size() ? to_string(normalized[i]) : "---") << "\t";
            os << (i < second_normalized.size() ? to_string(second_normalized[i]) : "---") << "\n";
        }

        if (normalized != second_normalized) {
            throw std::runtime_error("Normalization is not idempotent");
        }
    })

    .testCase("Normalize vector with lists", [](ostream& os) {
        // vMotif(ln(n(0.1), n(0.2)), lm(m(0.1), m(0.2)), la(a(0.1), a(0.2)), li(i(0.1), i(0.2)));
        std::vector<double> output, v = {
            1.000000, 0.472136, 1.000000, 0.988764, 1.000000, 0.270510, 
            1.000000, 0.090170, 0.510000, 0.389195, 0.000000, 1.000000, 
            0.090170, 0.510000, 0.720975, 0.000000, 0.000000, 1.000000, 
            0.506578, 1.000000, 0.326238, 0.530000, 0.000921, 0.000000, 
            1.000000, 0.326238, 0.530000, 0.000931, 0.000000, 0.000000, 
            1.000000, 0.742646, 1.000000, 0.562306, 0.550000, 0.001279, 
            0.000000, 1.000000, 0.562306, 0.550000, 0.001471, 0.000000, 
            0.000000, 1.000000, 0.360680, 1.000000, 0.180340, 0.560000, 
            0.000924, 0.000000, 1.000000, 0.180340, 0.560000, 0.000937, 
            0.000000, 0.000000, 0.000000, 0.000000
        };

        normalizeVector(v, output);

        os << "Original vector: " << humanReadableNormalizedVector(v) << endl << endl << endl;
        os << "Normalized vector: " << humanReadableNormalizedVector(output) << endl;
    })

    .testCase("Germinal vector to expression", [](ostream& os) {
        const auto tree = s({v({e_piano({n(1.5), m(120), a(30), i(4)})})});

        auto v = tree.toNormalizedVector();
        auto exp = toExpression(v);

        auto tree2 = parseString(exp);
        auto v2 = tree2.toNormalizedVector();

        os << tree.toString() << endl;
        os << to_string(v) << endl;
        os << exp << endl;
        os << tree2.toString() << endl;
        os << to_string(v2) << endl;

        printOutput();
    });

