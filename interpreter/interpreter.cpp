#include <iostream>
#include <istream>

#include "../library/genomus-core.hpp"

using namespace std;

int main() {
    string input;
    while (true) {
        getline(cin, input, ';');
        cout << input << endl;
    }
    return 0;
}