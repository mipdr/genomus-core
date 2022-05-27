#ifndef __GENOMUS_CORE_DECODED_GENOTYPE__
#define __GENOMUS_CORE_DECODED_GENOTYPE__ 

#include<functional>
#include <vector>

#include "encoded_phenotype.hpp"
#include "features.hpp"

using namespace std;

enum FunctionType {
    leafF,
    listF,
    defaultF,
};

/*
    GTree class is the ADT for decoded genotypes. Its instances will hold what is needed
    to instantiate and evaluate a decoded genotype.

    The instance itself represents a node in the function tree. It stores a reference to
    its parameters and the function to evaluate itself
*/
class GFunction : GenomusFeature {
    public:
    struct GFunctionInitializer {
        string name;
        vector<FunctionType> param_types;
        FunctionType type;
        function<enc_phen_t(vector<GFunction>)> compute;
    };

    private:
        // GenomusFeature fields
        string _name;
        FeatureType _type;

        // GFunction fields
        vector<FunctionType> _param_types;
        FunctionType _output_type;
        vector<GFunction*> _children;
        function<enc_phen_t(vector<GFunction>)> _compute;
    public:
        GFunction();
        GFunction(GFunctionInitializer);
        FunctionType type();
        enc_phen_t evaluate(vector<GFunction>);
        string toString();
        string toStringPretty();
};

// GFunction instances declaration
extern GFunction dec_gen_lvl_expl_function;

// GFunction instances initialization
void initialize_dec_gen_lvl_functions();

using dec_gen_t = GFunction;


#endif