#ifndef __GENOMUS_CORE_ENCODED_PHENOTYPE__
#define __GENOMUS_CORE_ENCODED_PHENOTYPE__ 

#include <functional>
#include <string>
#include <vector>
#include "species.hpp"

#define ENCODED_PHENOTYPES_TYPE_CHECK

using namespace std;

enum EncodedPhenotypeType {
    ept_leaf,
    ept_parameter,
    ept_event,
    ept_voice,
    ept_score
};

string EncodedPhenotypeTypeToString(EncodedPhenotypeType ept);

class EncodedPhenotype {
    public:
        struct EncodedPhenotypeInitializer {
            EncodedPhenotypeType type;
            EncodedPhenotypeType child_type;
            vector<EncodedPhenotype> children;
            function<string(vector<string>)> to_string;
            float leaf_value;
        };
    
    private:
        EncodedPhenotypeType _type;
        EncodedPhenotypeType _child_type;
        vector<EncodedPhenotype> _children;
        function<string(vector<string>)> _to_string;
        float _leaf_value;
    public:
        EncodedPhenotype(EncodedPhenotypeInitializer);
        EncodedPhenotypeType getType();
        EncodedPhenotypeType getChildType();
        string toString();
};

EncodedPhenotype Parameter(float value) ;
EncodedPhenotype Event(vector<EncodedPhenotype> parameters);
EncodedPhenotype Voice(vector<EncodedPhenotype> parameters);
EncodedPhenotype Score(vector<EncodedPhenotype> parameters);

using enc_phen_t = EncodedPhenotype;

#endif