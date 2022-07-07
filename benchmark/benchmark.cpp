#include <iostream>
#include <chrono>

#include "genomus-core.hpp"

using namespace std;

using sclock = std::chrono::system_clock;
using sec = std::chrono::duration<double, std::milli>;

int main() {
    init_genomus();
    
    std::vector<double> germinal, normalized;
    std::string expression;
    dec_gen_t tree = 0;
    double average_resulting_phenotype_length = 0;
    double total_ms = 10000;

    chrono::time_point<sclock> total_before, before;
    sec total_duration, duration;

    cout << "##########    GENOMUS-CORE BENCHMARK    ##########\n" << endl;
    cout << "Instances of germinal vector generation, normalization, parsing, evaluation and vector representation will be thrown with the following parameters:" << endl;

    cout << " - GERMINAL_VECTOR_MAX_LENGTH = " << GERMINAL_VECTOR_MAX_LENGTH << endl;
    cout << " - MAX_GENOTYPE_VECTOR_SIZE = " << MAX_GENOTYPE_VECTOR_SIZE << endl;
    cout << " - MAX_GENOTYPE_DEPTH = " << MAX_GENOTYPE_DEPTH << endl;
    cout << " - MAX_LIST_SIZE = " << MAX_LIST_SIZE << endl;

    total_before = sclock::now();

    size_t i = 0;
    while (true) {
        germinal = normalized = {};
        expression = "";
        GTree::clean(); 

        germinal = newGerminalVector();
        normalizeVector(germinal, normalized);
        expression = toExpression(normalized);
        tree = parseString(expression);

        average_resulting_phenotype_length += tree.evaluate().toNormalizedVector().size();

        i++;
        duration = sclock::now() - total_before;
        if (duration.count() > total_ms) {
            break;
        }
    }

    total_duration = sclock::now() - total_before;

    cout << "Benchmark completed " << i << " iterations in " << total_duration.count() << "ms" << endl;
    cout << "Average phenotype vector length: " << average_resulting_phenotype_length / i << endl;

    return 0;
}