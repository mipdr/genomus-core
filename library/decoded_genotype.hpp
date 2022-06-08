#ifndef __GENOMUS_CORE_DECODED_GENOTYPE__
#define __GENOMUS_CORE_DECODED_GENOTYPE__ 

#include <functional>
#include <vector>

#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "parameter_mapping.hpp"

using namespace std;

/*
    GFunction is the ADT chosen to represent the functions on the decoded genotype function tree.
    These functions are also genomus features, so they will be exposed in JS.

    Instances of GFunction are intended to be static. Built and initialized within the library.
*/
class GFunction : public GenomusFeature {
    public:
    struct GFunctionInitializer {
        string name;
        vector<EncodedPhenotypeType> param_types;
        EncodedPhenotypeType output_type;
        function<enc_phen_t(vector<enc_phen_t>)> compute;
        function<string(vector<string>)> build_explicit_form;
    };

    private:
        // GenomusFeature fields
        string _name;
        FeatureType _type;

        // GFunction fields
        vector<EncodedPhenotypeType> _param_types;
        EncodedPhenotypeType _output_type;
        function<enc_phen_t(vector<enc_phen_t>)> _compute;
        function<string(vector<string>)> _build_explicit_form;

        void _assert_parameter_format(const vector<enc_phen_t>&);
    public:
        GFunction();
        GFunction(const GFunction&);
        GFunction(GFunctionInitializer);

        vector<EncodedPhenotypeType> getParamTypes();
        function<string(vector<string>)> getBuildExplicitForm();

        EncodedPhenotypeType getOutputType();
        enc_phen_t evaluate(const vector<enc_phen_t>&);
        enc_phen_t operator()(const vector<enc_phen_t>&);
        string toString();
};

/*
    GTree class is the ADT for decoded genotypes. Its instances will hold what is needed
    to instantiate and evaluate a decoded genotype.

    GTree is an abstract class, derived by the GNode and GLeaf classes to reflect the 
    differences between function nodes and parameter leafs in the function tree.

    Instances of GTree are intended to be built at runtime.
*/

class GTree {
    private:
        GFunction& _function;
        vector<GTree*> _children;

        float _leaf_value; // Temporary, I just don't know where to put leaf values on trees
    public:
        GTree(GFunction&, vector<GTree*>, float leaf_value = 0);

        enc_phen_t evaluate();
        string toString();
};

using dec_gen_t = GTree;


// GFunction instances declaration
extern GFunction eventF, paramF;  

// GFunction instances initialization
void initialize_dec_gen_lvl_functions();


#endif