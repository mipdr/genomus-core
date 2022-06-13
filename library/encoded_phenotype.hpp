#ifndef __GENOMUS_CORE_ENCODED_PHENOTYPE__
#define __GENOMUS_CORE_ENCODED_PHENOTYPE__ 

#include <functional>
#include <string>
#include <vector>
#include "species.hpp"


#define ENCODED_PHENOTYPES_TYPE_CHECK

enum EncodedPhenotypeType {
    // Base types
    ept_leaf,
    ept_parameter,
    ept_event,
    ept_voice,
    ept_score,

    // Types for functions with arbitrary parameters
    ept_parameters,
    ept_events,
    ept_voices,
};

std::string EncodedPhenotypeTypeToString(const EncodedPhenotypeType& ept);

class EncodedPhenotype {
    public:
        struct EncodedPhenotypeInitializer {
            EncodedPhenotypeType type;
            EncodedPhenotypeType child_type;
            std::vector<EncodedPhenotype> children;
            std::function<std::string(std::vector<std::string>)> to_string;
            float leaf_value;
        };
    
    private:
        EncodedPhenotypeType _type;
        EncodedPhenotypeType _child_type;
        std::vector<EncodedPhenotype> _children;
        std::function<std::string(std::vector<std::string>)> _to_string;
        float _leaf_value;
    public:
        EncodedPhenotype(EncodedPhenotypeInitializer);
        EncodedPhenotypeType getType();
        EncodedPhenotypeType getChildType();
        std::string toString();
        float getLeafValue();
};

EncodedPhenotype Parameter(float value);
EncodedPhenotype Parameter(std::vector<EncodedPhenotype> parameters);
EncodedPhenotype Event(std::vector<EncodedPhenotype> parameters);
EncodedPhenotype Voice(std::vector<EncodedPhenotype> parameters);
EncodedPhenotype Score(std::vector<EncodedPhenotype> parameters);

using enc_phen_t = EncodedPhenotype;

#endif