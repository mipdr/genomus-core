#include<iostream>
#include "parameter_dictionary.hpp"

map<string, ParameterMapper> ParameterDictionary = {
    {"NoteValueF", NoteValueF()}
};

void example() {
    const NoteValueF mapper;
    const float norm = 0.5;

    const float unnorm = mapper << norm;
    cout << norm << "   " << unnorm << endl;
}