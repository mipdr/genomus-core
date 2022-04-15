#include<iostream>
#include "parameter_dictionary.hpp"

map<string, ParameterMapper> ParameterDictionary = {
    {"ParamF", ParamF},
    {"NoteValueF", NoteValueF},
    {"DurationF", DurationF},
};