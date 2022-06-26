#ifndef __GENOMUS_CORE_PARAMETER_MAPPING__
#define __GENOMUS_CORE_PARAMETER_MAPPING__

#include <functional>
#include <map>
#include "features.hpp"


struct ParameterMapperInitializer{
    std::string name;
    std::function<double(double)> encoder;
    std::function<double(double)> decoder;
};

class ParameterMapper : public GenomusFeature {
    private:
        std::string _name;
        FeatureType _type;
    public:
        std::function<double(double)> _encode = 0;
        std::function<double(double)> _decode = 0;
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
        double operator>>(const double p);
        double operator<<(const double p);

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