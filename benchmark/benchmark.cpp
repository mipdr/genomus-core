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
    size_t iterations = 10;
    double average_resulting_phenotype_length = 0;

    chrono::time_point<sclock> total_before, before;
    sec total_duration, duration;

    cout << "##########    GENOMUS-CORE BENCHMARK    ##########\n" << endl;
    cout << iterations << " instances of germinal vector generation, normalization, parsing, evaluation and vector representation will be thrown with the following parameters:" << endl;

    cout << " - GERMINAL_VECTOR_MAX_LENGTH = " << GERMINAL_VECTOR_MAX_LENGTH << endl;
    cout << " - MAX_GENOTYPE_VECTOR_SIZE = " << MAX_GENOTYPE_VECTOR_SIZE << endl;
    cout << " - MAX_GENOTYPE_DEPTH = " << MAX_GENOTYPE_DEPTH << endl;
    cout << " - MAX_LIST_SIZE = " << MAX_LIST_SIZE << endl;

    total_before = sclock::now();

    for (size_t i = 0; i < 10; ++i){
        germinal = normalized = {};
        expression = "";
        GTree::clean(); 

        germinal = newGerminalVector();
        normalizeVector(germinal, normalized);
        expression = toExpression(normalized);
        tree = parseString(expression);

        average_resulting_phenotype_length += tree.evaluate().toNormalizedVector().size();

        // cout << ((tree.toString().find("Autoref") != string::npos) ? "Contains autoreferences" : "Does not contain autoreferences") << endl;
    }

    total_duration = sclock::now() - total_before;

    cout << "Benchmark completed in " << total_duration.count() << "ms" << endl;
    cout << "Average phenotype vector length: " << average_resulting_phenotype_length / iterations << endl;

    return 0;
}