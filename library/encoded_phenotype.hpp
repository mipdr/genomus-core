#ifndef __GENOMUS_CORE_ENCODED_PHENOTYPE__
#define __GENOMUS_CORE_ENCODED_PHENOTYPE__ 

#include <functional>
#include <string>
#include <vector>
#include "species.hpp"


#define ENCODED_PHENOTYPES_TYPE_CHECK

enum EncodedPhenotypeType {
    scoreF,
    voiceF,
    eventF,
    listF,
    paramF,
    leafF,
    noteValueF,
    durationF,
    midiPitchF,
    frequencyF,
    articulationF,
    intensityF,
    goldenintegerF,
    quantizedF,
    lnoteValueF,
    ldurationF,
    lmidiPitchF,
    lfrequencyF,
    larticulationF,
    lintensityF,
    lgoldenintegerF,
    lquantizedF,

    harmonyF,
};

static const std::vector<EncodedPhenotypeType> listTypes = {
    lnoteValueF,
    ldurationF,
    lmidiPitchF,
    lfrequencyF,
    larticulationF,
    lintensityF,
    lgoldenintegerF,
    lquantizedF,
};

static const std::vector<EncodedPhenotypeType> parameterTypes = {
    noteValueF,
    durationF,
    midiPitchF,
    frequencyF,
    articulationF,
    intensityF,
    goldenintegerF,
    quantizedF,
};

std::string encodedPhenotypeTypeToString(const EncodedPhenotypeType& ept);

class EncodedPhenotype {
    public:
        struct EncodedPhenotypeInitializer {
            EncodedPhenotypeType type;
            EncodedPhenotypeType child_type;
            std::vector<EncodedPhenotype> children;
            std::function<std::string(std::vector<std::string>)> to_string;
            double leaf_value;
        };
    
    private:
        EncodedPhenotypeType _type;
        EncodedPhenotypeType _child_type;
        std::vector<EncodedPhenotype> _children;
        std::function<std::string(std::vector<std::string>)> _to_string;
        double _leaf_value;
    public:
        EncodedPhenotype(EncodedPhenotypeInitializer);
        EncodedPhenotypeType getType();
        EncodedPhenotypeType getChildType();
        std::string toString();
        const std::vector<EncodedPhenotype>& getChildren();
        double getLeafValue();
        std::vector<double> toNormalizedVector() const;
};

EncodedPhenotype Parameter(double value);
EncodedPhenotype Parameter(std::vector<EncodedPhenotype> parameters);
EncodedPhenotype Event(std::vector<EncodedPhenotype> parameters);
EncodedPhenotype Voice(std::vector<EncodedPhenotype> parameters);
EncodedPhenotype Score(std::vector<EncodedPhenotype> parameters);

using enc_phen_t = EncodedPhenotype;

#endif