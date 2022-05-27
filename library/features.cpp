#include "features.hpp"
#include "features.hpp"
#include "parameter_mapping.hpp"

std::string featureTypeToString(FeatureType ft) {
    switch (ft) {
        case parameter_mapper:
            return "parameter_mapper";
        case decoded_genotype_level_function:
            return "decoded_genotype_level_function";
        default:
            return "invalid_feature_type";
    }
}

std::string GenomusFeature::getName() { return this -> _name; }
FeatureType GenomusFeature::getType() { return this -> _type; }
std::string GenomusFeature::getTypeString() { return featureTypeToString(this -> _type); }

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