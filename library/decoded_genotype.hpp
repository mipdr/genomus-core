#ifndef __GENOMUS_CORE_DECODED_GENOTYPE__
#define __GENOMUS_CORE_DECODED_GENOTYPE__ 

#include <functional>
#include <vector>

#include "encoded_phenotype.hpp"
#include "features.hpp"
#include "parameter_mapping.hpp"

/*
    GTree class is the ADT for decoded genotypes. Its instances will hold what is needed
    to instantiate and evaluate a decoded genotype.

    GTree is an abstract class, derived by the GNode and GLeaf classes to reflect the 
    differences between function nodes and parameter leafs in the function tree.

    Instances of GTree are intended to be built at runtime.
*/

class GTree {
    public:

    class GTreeIndex {
        private:
            size_t _index;
        public:
            GTreeIndex(size_t);
            enc_phen_t evaluate();
            std::string toString();
            operator size_t() const;
            float getLeafValue();
            size_t getIndex();
            static void clean();
    };
    

    /*
        GFunction is the ADT chosen to represent the functions on the decoded genotype function tree.
        These functions are also genomus features, so they will be exposed in JS.

        Instances of GFunction are intended to be static. Built and initialized within the library.
    */
    class GFunction : public GenomusFeature {
        public:
        struct GFunctionInitializer {
            std::string name;
            std::vector<EncodedPhenotypeType> param_types;
            EncodedPhenotypeType output_type;
            std::function<enc_phen_t(std::vector<enc_phen_t>)> compute;
            std::function<std::string(std::vector<std::string>)> build_explicit_form;
        };

        private:
            // GenomusFeature fields
            std::string _name;
            FeatureType _type;

            // GFunction fields
            std::vector<EncodedPhenotypeType> _param_types;
            EncodedPhenotypeType _output_type;
            std::function<enc_phen_t(std::vector<enc_phen_t>)> _compute;
            std::function<std::string(std::vector<std::string>)> _build_explicit_form;
            bool _is_autoreference;

            void _assert_parameter_format(const std::vector<enc_phen_t>&);
        public:
            GFunction();
            GFunction(const GFunction&);
            GFunction(GFunctionInitializer);

            std::vector<EncodedPhenotypeType> getParamTypes();
            // function<string(vector<string>)>& getBuildExplicitForm();
            std::string buildExplicitForm(std::vector<std::string>);

            EncodedPhenotypeType getOutputType() const;
            bool getIsAutoreference() const;
            enc_phen_t evaluate(const std::vector<enc_phen_t>&);
            // enc_phen_t operator()(const std::vector<enc_phen_t>&);
            GTreeIndex operator()(std::initializer_list<GTreeIndex>);
            GTreeIndex operator()(const std::vector<GTreeIndex>);
            GTreeIndex operator()(float);
            std::string toString();
    };
    
    private:
        GFunction& _function;
        std::vector<GTreeIndex> _children;

        float _leaf_value; // Temporary, I just don't know where to put leaf values on trees
    public:
        static std::vector<GTree> tree_nodes;
        static std::map<EncodedPhenotypeType, std::vector<GTree::GTreeIndex>> available_subexpressions;
        static EncodedPhenotype evaluateAutoReference(EncodedPhenotypeType, size_t index);
        static void registerLastInsertedNodeAsSubexpression();
        static std::string printStaticData();
        static void clean();
        GTree(GFunction&, std::vector<GTreeIndex>, float leaf_value = 0);

        enc_phen_t evaluate();
        std::string toString();
};

using dec_gen_t = GTree::GTreeIndex;

// GFunction instances declaration
extern GTree::GFunction
    p,
    l,
    e,
    v,
    s,
    n,
    d,
    m,
    // f,
    a,
    i,
    // z,
    // q,
    e_piano,
    vConcatE,
    vConcatV,
    eAutoRef
    ; 

#endif