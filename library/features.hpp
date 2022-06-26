#ifndef __GENOMUS_CORE_FEATURES__
#define __GENOMUS_CORE_FEATURES__

#include <string>
#include <vector>

enum FeatureType {
    parameter_mapper = 0,
    decoded_genotype_level_function = 1,
};

std::string featureTypeToString(FeatureType ft);

class GenomusFeature {
    private:
        std::string _name;
        FeatureType _type;
    public:
        virtual std::string getName();
        virtual FeatureType getType();
        virtual std::string getTypeString();
};

// extern std::vector<GenomusFeature*> genomusFeatures;

#endif