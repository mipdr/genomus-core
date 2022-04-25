#include "genomus-core.hpp"
#include "testing_utils/testing_utils.hpp"

/*
    Just a primitive test file example.
    This will be improved in the future.
*/

void test_case_example_1() {
    // This gives out an error
    assert(NoteValueF << 1.0, 0.0);
}

int main() {
    test_case_example_1();
    return 0;
}