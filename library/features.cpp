#include "features.hpp"
#include "features.hpp"
#include "parameter_mapping.hpp"

std::string GenomusFeature::getName() { return this -> _name; }
FeatureType GenomusFeature::getType() { return this -> _type; }

std::vector<GenomusFeature*> genomusFeatures({
    &NoteValueF, 
    &ParamF,
    &DurationF,
    &MidiPitchF,
    &FrequencyF,
    &ArticulationF,
    &IntensityF,
    &GoldenintegerF,
    &QuantizedF
});