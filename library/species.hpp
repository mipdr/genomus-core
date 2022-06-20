#ifndef __GENOMUS_CORE_SPECIES__
#define __GENOMUS_CORE_SPECIES__ 

#include <string>
#include <vector>

enum ParameterType {
    noteValue,
    duration,
    pitch,
    articulation,
    intensity,
};

std::string ParameterTypeToString(ParameterType);

class Species {
    struct SpeciesInitializer {
        std::string name;
        std::vector<ParameterType> parameter_types;
    };

    private:
        std::string _name;
        std::vector<ParameterType> _parameter_types;
    public:
        Species();
        Species(SpeciesInitializer);
        std::string getName();
        std::vector<ParameterType> getParameterTypes();
        std::string toString();
};

extern Species CURRENT_SPECIES,
    piano;

void usePianoSpecies();

#endif