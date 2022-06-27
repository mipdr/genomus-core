#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "testing_utils.hpp"

using namespace std;

int main() {
    std::vector<GTest> tests({
        EncodedPhenotypesTest,
        DecodedGenotypesTest,
        ParserTest,
        EncodedGenotypesTest
    });

    GTestErrorState result = g_success;

    try {
        for (auto& test: tests) {
            if (test.run() == g_failure)
                result = g_failure;
        }

    } catch (runtime_error e) {
        cout << e.what() << endl;
        return 1; 
    }

    if (result == g_failure) {
        return 1;
    }
    
    return 0;
}