#ifndef __GENOMUS_CORE_ENCODED_PHENOTYPE__
#define __GENOMUS_CORE_ENCODED_PHENOTYPE__ 

#include <string>
#include <vector>
#include "species.hpp"

using namespace std;

enum EncodedPhenotypeType {
    ept_parameter,
    ept_event,
    ept_voice,
    ept_score
};

string EncodedPhenotypeTypeToString(EncodedPhenotypeType ept);

class EncodedPhenotype {
    virtual EncodedPhenotypeType getEptType();
};

class Parameter : public EncodedPhenotype {
    public:
        struct ParameterInitializer {
            ParameterType parameter_type;
            float value;
        };

    private:
        ParameterType _parameter_type;
        float _value;
    public:
        EncodedPhenotypeType getEptType() override;

        Parameter(ParameterInitializer);
        ParameterType getParameterType();
        float getValue();
};

class Event : public EncodedPhenotype {
    public:
    struct EventInitializer {
        vector<Parameter> parameters;
    };

    private:
        vector<Parameter> _parameters;
    public:
        EncodedPhenotypeType getEptType() override;

        Event(EventInitializer, Species s = CURRENT_SPECIES);
        vector<ParameterType> getParameterTypes();
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
        EncodedPhenotypeType getEptType() override;

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
        EncodedPhenotypeType getEptType() override;

        Score(ScoreInitializer);
        vector<Voice> getVoices();
        string toString(bool pretty_print = false);
};

using enc_phen_t = EncodedPhenotype;

#endif