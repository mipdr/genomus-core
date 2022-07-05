#include <iostream>
#include <stdexcept>

#include "genomus-core.hpp"
#include "testing_utils.hpp"

GTest ParserTest = GTest("Parser test")

    .before([]() { init_genomus(); })
    .beforeEach([]() { GTree::clean(); })
    .after([]() { GTree::clean(); })

    .testCase("Check parser tokens", [](ostream& os) {
        auto entry = "vConcatV(vConcatE(e(n(1.0), m(2.0), a(3.0), i(1)), eAutoref(0)), vConcatE(eAutoref(1), eAutoref(2)))";
        auto tree = parseString(entry);

        os << "Entry: " << entry << "\n\n";
        os << "Decoded Genotype: " << tree.toString() << "\n\n";
        os << "Encoded Phenotype: " << tree.evaluate().toString() << "\n\n";
    })

    .testCase("Genotype to string to genotype", [](ostream& os) {
        auto tree = vConcatV({vConcatE({e({n(1.0), m(2.0), a(3.0), i(1)}), eAutoref(0)}), vConcatE({eAutoref(1), eAutoref(2)})});
        auto tree_string = tree.toString();
        auto parsed_tree = parseString(tree_string);
        auto parsed_tree_string = parsed_tree.toString();

        if (tree_string != parsed_tree_string) {
            std::string error_message = "Assertion error: expected\n\n" + tree_string + "\n\nto equal\n\n" + parsed_tree_string + "\n";
            throw runtime_error(error_message);
        }
    });