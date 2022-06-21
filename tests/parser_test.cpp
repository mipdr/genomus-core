#include "decoded_genotype.hpp"
#include "testing_utils.hpp"
#include "parser.hpp"
#include "encoded_genotype.hpp"
#include <iostream>

GTest ParserTest = GTest("Parser test")

    .before([]() { init_available_functions(); })

    .testCase("Check parser tokens", [](ostream& os) {
        auto entry = "vConcatV(vConcatE(e_piano(n(1.0), m(2.0), a(3.0), i(1)), eAutoRef(0)), vConcatE(eAutoRef(1), eAutoRef(2)))";
        auto tree = parseString(entry);

        os << "Entry: " << entry << "\n";
        os << "Decoded Genotype: " << tree.toString() << "\n";
        os << "Encoded Phenotype: " << tree.evaluate().toString() << "\n";

        printOutput(); 
    });