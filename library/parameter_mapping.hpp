#ifndef __GENOMUS_CORE_PARAMETER_MAPPING__
#define __GENOMUS_CORE_PARAMETER_MAPPING__

#include <functional>
#include <map>
#include "features.hpp"


struct ParameterMapperInitializer{
    std::string name;
    std::function<float(float)> encoder;
    std::function<float(float)> decoder;
};

class ParameterMapper : public GenomusFeature {
    private:
        std::string _name;
        FeatureType _type;
        std::function<float(float)> _encode = 0;
        std::function<float(float)> _decode = 0;
    public:
        /*
            Default constructor is only implemented for stl container requirements.
            It instantiates a default mapper made out of identity functions.
        */
        ParameterMapper();
        ParameterMapper(ParameterMapperInitializer);

        /*
            Extraction and insertion operators are overloaded in this library to add
            some sintax sugar to the code reflecting encoding and decoding of objects.

             - Genomus encoding operator <<
             - Genomus decoding operator >>
        */
        float operator>>(const float p);
        float operator<<(const float p);

        // Accessors
        std::string getName();
        FeatureType getType();
};

/*
    Parameter mapper functions. This set of functions consists of those Genomus functions
    that take a single number as input and output a single number.
*/
extern ParameterMapper 
    NoteValueF, 
    ParamF,
    DurationF,
    MidiPitchF,
    FrequencyF,
    ArticulationF,
    IntensityF,
    GoldenintegerF,
    QuantizedF;

#endif