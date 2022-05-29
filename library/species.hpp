#ifndef __GENOMUS_CORE_SPECIES__
#define __GENOMUS_CORE_SPECIES__ 

#include <string>
#include <vector>

using namespace std;

enum ParameterType {
    duration,
    pitch,
    articulation,
    intensity,
};

string ParameterTypeToString(ParameterType);

class Species {
    struct SpeciesInitializer {
        string name;
        vector<ParameterType> parameter_types;
    };

    private:
        string _name;
        vector<ParameterType> _parameter_types;
    public:
        Species();
        Species(SpeciesInitializer);
        string getName();
        vector<ParameterType> getParameterTypes();
        string toString();
};

extern Species CURRENT_SPECIES,
    piano;

void usePianoSpecies();

#endif