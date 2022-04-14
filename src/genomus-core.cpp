#include <iostream>
#include "genomus-core.hpp"

#include "parameter_dictionary/parameter_dictionary.hpp"

using namespace std;

GenoMus::GenoMus() {
    this -> sample_member = 0;
}

void GenoMus::hello() {


    cout << "Hello World" << endl;
    // cout << "Probamos a codificat y decodificar: ";

    example();

}
