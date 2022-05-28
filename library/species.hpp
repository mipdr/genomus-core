#ifndef __GENOMUS_CORE_SPECIES__
#define __GENOMUS_CORE_SPECIES__ 

#include <string>
#include <vector>

using namespace std;

enum EventParameterType {
    duration,
    pitch,
    articulation,
    intensity,
};

string eventParameterTypeToString(EventParameterType);

class Species {
    struct SpeciesInitializer {
        string name;
        vector<EventParameterType> parameter_types;
    };

    private:
        string _name;
        vector<EventParameterType> _parameter_types;
    public:
        Species();
        Species(SpeciesInitializer);
        string getName();
        vector<EventParameterType> getParameterTypes();
        string toString();
};

extern Species CURRENT_SPECIES,
    piano;

void usePianoSpecies();

#endif