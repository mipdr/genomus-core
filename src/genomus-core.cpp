#include <iostream>
#include "genomus-core.hpp"
#include "phenotype/phenotype.hpp"

using namespace std;
using namespace Phenotype;

GenoMus::GenoMus() {
    this -> sample_member = 0;
}

void GenoMus::hello() {
    cout << "Hello World" << endl;
    EncodedPhenotype<Piano> pheno;
    cout << pheno.hello() << endl;
}
