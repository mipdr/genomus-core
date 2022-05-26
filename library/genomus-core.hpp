#ifndef __GENOMUS_CORE__
#define __GENOMUS_CORE__

#include "parameter_mapping.hpp"
#include "features.hpp"
#include "decoded_genotype.hpp"

class GenoMus {
    private:
        int sample_member;
    public:
        GenoMus();
        void hello();
};

#endif