#include <iostream>
#include <stdexcept>

#include "tests.hpp"

using namespace std;

int main() {
    try {
        EncodedPhenotypesTest::run();
        //DecodedGenotypesTest::run();
    } catch (runtime_error e) {
        cout << e.what() << endl;
        return 1;
    }
    
    return 0;
}