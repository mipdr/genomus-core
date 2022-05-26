#ifndef __GENOMUS_CORE_SPECIMEN__
#define __GENOMUS_CORE_SPECIMEN__ 

#include <vector>
#include "g_types.hpp"

using namespace std;

class Specimen {
    private:
        enc_gen_t _encoded_genotype;
        dec_gen_t _decoded_genotype;
        enc_phen_t _encoded_phenotype;
        dec_phen_t _decoded_phenotype;
    public:
        Specimen(enc_gen_t);
        enc_gen_t getEncodedGenotype();
        dec_gen_t getDecodedGenotype();
        enc_phen_t getEncodedPhenotype();
        dec_phen_t getDecodedPhenotype();
};

#endif