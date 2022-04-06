#ifndef __GENOMUS_CORE_PARAMETER_MAPPING__
#define __GENOMUS_CORE_PARAMETER_MAPPING__

namespace ParameterMapping {
    /*
        Parameter mapping will be done by the use of functors.
        Proposal:
            - Encoding a parameter p:
                const float normalized_p = notevalueF << p;
                const float p = notevalueF >> normalized_p;
    */

    class ParameterMapper {
        private:
            virtual float _encode(float generic_value);
            virtual float _decode(float normalized_value);
        public:
            virtual float operator>>(float p) final;
            virtual float operator<<(float p) final;
    };

    // class NoteValueF : ParameterMapper {};
    // class MidiPitchF : ParameterMapper {};
    // class FrequencyF : ParameterMapper {};
    // class ArticulationF : ParameterMapper {};
    // class IntensityF : ParameterMapper {};
}

#endif