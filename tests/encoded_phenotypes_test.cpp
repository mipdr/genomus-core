#include "encoded_phenotype.hpp"
#include "genomus-core.hpp"
#include <iostream>
#include "species.hpp"
#include "tests.hpp"

using namespace std;

void EncodedPhenotypesTest::run() {
    auto e = Event({
        .parameters = {}
    }, CURRENT_SPECIES);

    cout << CURRENT_SPECIES.toString() << endl;
}
