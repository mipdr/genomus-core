#include <iostream>
#include "genomus-core.hpp"

#include "parameter_dictionary/parameter_dictionary.hpp"
#include "parameter_dictionary/parameter_mapping/parameter_mapping.hpp"

using namespace std;

GenoMus::GenoMus() {
    this -> sample_member = 0;
}

// Sample function for tests
void GenoMus::hello() {
    cout << "Hello world from genomus-core library" << endl;

    const ParameterMapper mapper = NoteValueF;
    const float norm = 0.5;

    const float unnorm = mapper << norm;
    cout << norm << "   " << unnorm << endl;

}
