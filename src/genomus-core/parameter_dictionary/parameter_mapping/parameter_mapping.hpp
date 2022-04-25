#ifndef __GENOMUS_CORE_PARAMETER_MAPPING__
#define __GENOMUS_CORE_PARAMETER_MAPPING__

#include <functional>
#include <map>
using namespace std;

struct ParameterMapperInitializer{
    string name;
    function<float(float)> encoder;
    function<float(float)> decoder;
};

class ParameterMapper {
    private:
        function<float(float)> _encode;
        function<float(float)> _decode;
        string _name;
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