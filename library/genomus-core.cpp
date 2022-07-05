#include "genomus-core.hpp"
#include <ctime>

#include "decoded_genotype.hpp"

void init_genomus() {
    std::srand(std::time(0));
    init_available_functions();
}