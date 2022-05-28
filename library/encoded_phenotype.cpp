#include "encoded_phenotype.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

Event::Event(EventInitializer init, Species s) {
    if (init.parameter_types != s.getParameterTypes()) {
        throw runtime_error("Event initializer does not match event species.");
    }
    if (init.parameter_types.size() != init.parameter_values.size()) {
        throw runtime_error("Event parameter values and types cannot be of different length.");
    }
    this -> _parameter_types = init.parameter_types;
    this -> _parameter_values = init.parameter_values;
}

vector<EventParameterType> Event::getParameterTypes() { return this -> _parameter_types; }
vector<float> Event::getParameterValues() { return this -> _parameter_values; }

string Event::toString(bool pretty_print, int indentation_level) {
    string indentation = pretty_print ? string(indentation_level, '\t') : "";
    string ret = indentation + "e({";
    for (int i = 0; i < this -> _parameter_types.size(); ++i) {
        ret += eventParameterTypeToString(this -> _parameter_types[i]) + ": " + to_string(this -> _parameter_values[i]) + ", ";
    }
    return ret.substr(0, ret.length() - 2) + "})";
}

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