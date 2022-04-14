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
            virtual float _encode(float generic_value) const;
            virtual float _decode(float normalized_value) const;
        public:
            virtual const float operator>>(const float p) const final;
            virtual const float operator<<(const float p) const final;
    };

    class NoteValueF : public ParameterMapper {
        private:
            float _encode(float generic_value) const;
            float _decode(float normalized_value) const;
    };
    // class MidiPitchF : ParameterMapper {};
    // class FrequencyF : ParameterMapper {};
    // class ArticulationF : ParameterMapper {};
    // class IntensityF : ParameterMapper {};
}

#endif