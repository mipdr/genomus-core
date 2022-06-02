#ifndef __GENOMUS_CORE_ENCODED_PHENOTYPE__
#define __GENOMUS_CORE_ENCODED_PHENOTYPE__ 

#include <functional>
#include <string>
#include <vector>
#include "species.hpp"

#define ENCODED_PHENOTYPES_TYPE_CHECK

using namespace std;

enum EncodedPhenotypeType {
    ept_leaf,
    ept_parameter,
    ept_event,
    ept_voice,
    ept_score
};

string EncodedPhenotypeTypeToString(EncodedPhenotypeType ept);

class EncodedPhenotype {
    public:
        struct EncodedPhenotypeInitializer {
            EncodedPhenotypeType type;
            EncodedPhenotypeType child_type;
            vector<EncodedPhenotype> children;
            function<string(vector<string>)> to_string;
            float leaf_value;
        };
    
    private:
        EncodedPhenotypeType _type;
        EncodedPhenotypeType _child_type;
        vector<EncodedPhenotype> _children;
        function<string(vector<string>)> _to_string;
        float _leaf_value;
    public:
        EncodedPhenotype(EncodedPhenotypeInitializer);
        EncodedPhenotypeType getType();
        EncodedPhenotypeType getChildType();
        string toString();
};

EncodedPhenotype Parameter(float value) ;
EncodedPhenotype Event(vector<EncodedPhenotype> parameters);
EncodedPhenotype Voice(vector<EncodedPhenotype> parameters);

// class Parameter : public EncodedPhenotype {
//     public:
//         struct ParameterInitializer {
//             ParameterType parameter_type;
//             float value;
//         };

//     private:
//         ParameterType _parameter_type;
//         float _value;
//     public:
//         EncodedPhenotypeType getEptType() override;

//         Parameter(ParameterInitializer);
//         ParameterType getParameterType();
//         float getValue();
// };

// class Event : public EncodedPhenotype {
//     public:
//     struct EventInitializer {
//         vector<EncodedPhenotype> parameters;
//     };

//     private:
//         vector<Parameter> _parameters;
//     public:
//         EncodedPhenotypeType getEptType() override;

//         Event(EventInitializer);
//         vector<ParameterType> getParameterTypes();
//         vector<float> getParameterValues();
//         string toString(bool pretty_print = false, int indentation_level = 0);
// };

// class Voice : public EncodedPhenotype {
//     struct VoiceInitializer {
//         vector<EncodedPhenotype> events;
//     };

//     private:
//         vector<Event> _events;
//     public:
//         EncodedPhenotypeType getEptType() override;

//         Voice(VoiceInitializer);
//         vector<Event> getEvents();
//         string toString(bool pretty_print = false, int indentation_level = 0);
// };

// class Score : public EncodedPhenotype {
//     struct ScoreInitializer {
//         vector<EncodedPhenotype> voices;
//     };

//     private:
//         vector<Voice> _voices;
//     public:
//         EncodedPhenotypeType getEptType() override;

//         Score(ScoreInitializer);
//         vector<Voice> getVoices();
//         string toString(bool pretty_print = false);
// };

using enc_phen_t = EncodedPhenotype;

#endif