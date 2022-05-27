#ifndef __GENOMUS_CORE_DECODED_GENOTYPE__
#define __GENOMUS_CORE_DECODED_GENOTYPE__ 

#include<functional>
#include <vector>

#include "encoded_phenotype.hpp"
#include "features.hpp"

using namespace std;

enum FunctionType {
    paramF,
    listF,
    defaultF,
};

/*
    GFunction is the ADT chosen to represent the functions on the decoded genotype function tree.
    These functions are also genomus features, so they will be exposed in JS.
*/
class GFunction : GenomusFeature {
    public:
    struct GFunctionInitializer {
        string name;
        vector<FunctionType> param_types;
        FunctionType type;
        function<enc_phen_t(vector<enc_phen_t>)> compute;
        function<string(vector<string>)> build_explicit_form;
    };

    private:
        // GenomusFeature fields
        string _name;
        FeatureType _type;

        // GFunction fields
        vector<FunctionType> _param_types;
        FunctionType _output_type;
        vector<GFunction*> _children;
        function<enc_phen_t(vector<enc_phen_t>)> _compute;
        function<string(vector<string>)> _build_explicit_form;
    public:
        GFunction();
        GFunction(GFunctionInitializer);
        FunctionType type();
        enc_phen_t evaluate(vector<enc_phen_t>);
        string toString();
};

/*
    GTree class is the ADT for decoded genotypes. Its instances will hold what is needed
    to instantiate and evaluate a decoded genotype.

    The instance itself represents a node in the function tree. It stores a reference to
    its parameters and the function to evaluate itself.
*/

class GTree {
    private:
        GFunction _function;
        vector<GTree*> _children;
        bool _isLeaf;
    public:
        enc_phen_t evaluate();
        string toString();
};


// GFunction instances declaration
extern GFunction dec_gen_lvl_expl_function;

// GFunction instances initialization
void initialize_dec_gen_lvl_functions();

using dec_gen_t = GTree;


#endif