#ifndef __GENOMUS_CORE_DECODED_GENOTYPE__
#define __GENOMUS_CORE_DECODED_GENOTYPE__ 

#include<functional>
#include <vector>

#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "parameter_mapping.hpp"

using namespace std;

enum FunctionType {
    paramF,
    listF,
    defaultF,
};

/*
    GFunction is the ADT chosen to represent the functions on the decoded genotype function tree.
    These functions are also genomus features, so they will be exposed in JS.

    Instances of GFunction are intended to be static. Built and initialized within the library.
*/
class GFunction : public GenomusFeature {
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
        function<enc_phen_t(vector<enc_phen_t>)> _compute;
        function<string(vector<string>)> _build_explicit_form;
    public:
        GFunction();
        GFunction(GFunctionInitializer);

        vector<FunctionType> getParamTypes();
        function<string(vector<string>)> getBuildExplicitForm();

        FunctionType type();
        enc_phen_t evaluate(vector<enc_phen_t>);
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
    public:
        virtual enc_phen_t evaluate() = 0;
        virtual string toString() = 0;
};

class GNode : public GTree {
    private:
        GFunction* _function;
        vector<GTree*> _children;
    public:
        virtual enc_phen_t evaluate() override;
        virtual string toString() override;
};

class GLeaf : public GTree {
    private:
        ParameterMapper* _function;
        float _param;
    public:
        enc_phen_t evaluate() override;
        string toString() override;
};


// GFunction instances declaration
extern GFunction dec_gen_lvl_expl_function;

// GFunction instances initialization
void initialize_dec_gen_lvl_functions();

using dec_gen_t = GTree;


#endif