#ifndef __GENOMUS_CORE_ENCODED_PHENOTYPE__
#define __GENOMUS_CORE_ENCODED_PHENOTYPE__ 

#include <string>
#include <vector>
#include "species.hpp"

using namespace std;

class EncodedPhenotype {};

class Event : public EncodedPhenotype {
    public:
    struct EventInitializer {
        vector<EventParameterType> parameter_types;
        vector<float> parameter_values;
    };

    private:
        vector<EventParameterType> _parameter_types;
        vector<float> _parameter_values;
    public:
        Event(EventInitializer, Species s = CURRENT_SPECIES);
        vector<EventParameterType> getParameterTypes();
        vector<float> getParameterValues();
        string toString(bool pretty_print = false, int indentation_level = 0);
};

class Voice : public EncodedPhenotype {
    struct VoiceInitializer {
        vector<Event> events;
    };

    private:
        vector<Event> _events;
    public:
        Voice(VoiceInitializer);
        vector<Event> getEvents();
        string toString(bool pretty_print = false, int indentation_level = 0);
};

class Score : public EncodedPhenotype {
    struct ScoreInitializer {
        vector<Voice> voices;
    };

    private:
        vector<Voice> _voices;
    public:
        Score(ScoreInitializer);
        vector<Voice> getVoices();
        string toString(bool pretty_print = false);
};

using enc_phen_t = EncodedPhenotype;

#endif