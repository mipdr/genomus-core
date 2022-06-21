#include <iostream>
#include <stdexcept>

#include "testing_utils.hpp"

using namespace std;

int main() {
    try {
        // EncodedPhenotypesTest.run();
        // DecodedGenotypesTest.run();
        ParserTest.run();
    } catch (runtime_error e) {
        cout << e.what() << endl;
        return 1;
    }
    
    return 0;
}