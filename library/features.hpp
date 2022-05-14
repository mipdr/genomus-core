#ifndef __GENOMUS_CORE_FEATURES__
#define __GENOMUS_CORE_FEATURES__

#include <string>
#include <vector>

enum FeatureType {
    parameterMapper,
};

class GenomusFeature {
    private:
        std::string _name;
        FeatureType _type;
    public:
        std::string getName();
        FeatureType getType();
};

extern std::vector<GenomusFeature*> genomusFeatures;

#endif