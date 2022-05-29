#include "encoded_phenotype.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

string EncodedPhenotypeTypeToString(EncodedPhenotypeType ept) {
    switch (ept) {
        case ept_parameter:
            return "ept_parameter";
        case ept_event:
            return "ept_event";
        case ept_voice:
            return "ept_voice";
        case ept_score:
            return "ept_score";
        default:
            throw runtime_error("Invalid encoded phenotype type.");
    }
}

EncodedPhenotypeType Parameter::getEptType() { return ept_parameter; }

Parameter::Parameter(ParameterInitializer init) {
    this -> _parameter_type = init.parameter_type;
    this -> _value = init.value;
}

ParameterType Parameter::getParameterType() { return this -> _parameter_type; }
float Parameter::getValue() { return this -> _value; }

EncodedPhenotypeType Event::getEptType() { return ept_event; }

Event::Event(EventInitializer init, Species s) {
    vector<ParameterType> types;
    transform(init.parameters.begin(), init.parameters.end(), types.begin(), [](Parameter p){ return p.getParameterType(); });

    if (types != s.getParameterTypes()) {
        throw runtime_error("Event initializer does not match event species.");
    }

    this -> _parameters = init.parameters;
}

vector<ParameterType> Event::getParameterTypes() { 
    vector<ParameterType> types;
    transform(this -> _parameters.begin(), this -> _parameters.end(), types.begin(), [](Parameter p){ return p.getParameterType(); });
    return types;
}

vector<float> Event::getParameterValues() { 
    vector<float> values;
    transform(this -> _parameters.begin(), this -> _parameters.end(), values.begin(), [](Parameter p){ return p.getValue(); });
    return values;
}

string Event::toString(bool pretty_print, int indentation_level) {
    string indentation = pretty_print ? string(indentation_level, '\t') : "";
    string ret = indentation + "e({";
    for (int i = 0; i < this -> _parameters.size(); ++i) {
        ret += ParameterTypeToString(this -> _parameters[i].getParameterType()) + ": " + to_string(this -> _parameters[i].getValue()) + ", ";
    }
    return ret.substr(0, ret.length() - 2) + "})";
}

EncodedPhenotypeType Voice::getEptType() { return ept_voice; }

Voice::Voice(VoiceInitializer init) {
    this -> _events = init.events;
}

vector<Event> Voice::getEvents() { return this -> _events; }

string Voice::toString(bool pretty_print, int indentation_level) {
    string indentation = pretty_print ? string(indentation_level, '\t') : "";
    string ret = indentation + "v([";
    for_each(this -> _events.begin(), this -> _events.end(), [&](Event e) {
        ret += (pretty_print ? "\n" : "") + e.toString(pretty_print, indentation_level + 1) + ", ";
    });
    return ret.substr(0, ret.length() - 2) + (pretty_print ? "\n" + indentation : "") + "])";
}

EncodedPhenotypeType Score::getEptType() { return ept_score; }

Score::Score(ScoreInitializer init) {
    this -> _voices = init.voices;
}

vector<Voice> Score::getVoices() { return this -> _voices; }

string Score::toString(bool pretty_print) {
    string ret = "s([";
    for_each(this -> _voices.begin(), this -> _voices.end(), [&](Voice v) {
        ret += (pretty_print ? "\n" : "") + v.toString(pretty_print, 1) + ", ";
    });
    return ret.substr(0, ret.length() - 2) + (pretty_print ? "\n" : "") + "])";
}