#ifndef __GENOMUS_CORE_DECODED_GENOTYPE__
#define __GENOMUS_CORE_DECODED_GENOTYPE__ 

#include <functional>
#include <vector>
#include <map>

#include "encoded_phenotype.hpp"
#include "features.hpp"

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
            std::string toString() const;
            operator size_t() const;
            operator std::string() const;
            double getLeafValue() const;
            size_t getIndex() const;
            static void clean();
            std::vector<double> toNormalizedVector() const;
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
            size_t index;
            std::vector<EncodedPhenotypeType> param_types;
            EncodedPhenotypeType output_type;
            std::function<enc_phen_t(std::vector<enc_phen_t>)> compute;
            bool default_function_for_type;
        };

        private:
            // GenomusFeature fields
            std::string _name;
            FeatureType _type;

            // GFunction fields
            size_t _index;
            std::vector<EncodedPhenotypeType> _param_types;
            EncodedPhenotypeType _output_type;
            std::function<enc_phen_t(std::vector<enc_phen_t>)> _compute;
            bool _is_autoreference;
            bool _default_function_for_type;

            void _assert_parameter_format(const std::vector<enc_phen_t>&) const;
        public:
            std::string getName();

            GFunction();
            GFunction(const GFunction&);
            GFunction(const GFunction&, std::string name);
            GFunction(GFunctionInitializer);

            GFunction alias(std::string alias_name);

            std::vector<EncodedPhenotypeType> getParamTypes() const;
            size_t getIndex() const;
            std::string buildExplicitForm(std::vector<std::string>);

            EncodedPhenotypeType getOutputType() const;
            bool getIsAutoreference() const;
            bool getIsDefaultForType() const;
            enc_phen_t evaluate(const std::vector<enc_phen_t>&) const;
            GTreeIndex operator()(std::initializer_list<GTreeIndex>);
            GTreeIndex operator()(const std::vector<GTreeIndex>);
            GTreeIndex operator()(double);
            std::string toString();
    };
    
    private:
        GFunction& _function;
        std::vector<GTreeIndex> _children;

        double _leaf_value; // Temporary, I just don't know where to put leaf values on trees
    public:
        static std::vector<GTree> tree_nodes;
        static std::map<EncodedPhenotypeType, std::vector<GTree::GTreeIndex>> available_subexpressions;
        static EncodedPhenotype evaluateAutoReference(EncodedPhenotypeType, size_t index);
        static void registerLastInsertedNodeAsSubexpression();
        static std::string printStaticData();
        static void clean();

        GTree(GFunction&, std::vector<GTreeIndex>, double leaf_value = 0);

        enc_phen_t evaluate() const;
        std::vector<double> toNormalizedVector() const;
        std::string toString();
};

using dec_gen_t = GTree::GTreeIndex;

#define GENOTYPE_FUNCTIONS \
    p, e, v, s, \
    n, d, m, a, i, q, z, \
    ln, ld, lm, la, li, \
    vConcatE, \
    vConcatV, \
    vMotif, \
    e_piano, \
    eAutoRef
    // f, \

// GFunction instances declaration
extern GTree::GFunction GENOTYPE_FUNCTIONS;

// Function access structures

extern std::map<std::string, std::string> name_aliases;

static const double invalid_function_index = -1;
extern std::map<double, GTree::GFunction> available_functions;

using FunctionTypeDictionary = std::map<EncodedPhenotypeType, std::vector<double>>;
extern FunctionTypeDictionary function_type_dictionary;
extern FunctionTypeDictionary default_function_type_dictionary;

extern std::map<std::string, double> function_name_to_index;
void init_available_functions(); 


// utils

bool isEncodedPhenotypeTypeAParameterType(EncodedPhenotypeType);
bool isEncodedPhenotypeTypeAListType(EncodedPhenotypeType);
bool gfunctionAcceptsNumericParameter(const GTree::GFunction&);

double leafTypeToNormalizedValue(EncodedPhenotypeType);
EncodedPhenotypeType listToParameterType(EncodedPhenotypeType);

std::string humanReadableNormalizedVector(std::vector<double> v);

extern std::map<EncodedPhenotypeType, dec_gen_t> default_genotypes;

double encodeParameter(EncodedPhenotypeType parameterType, double value);
double decodeParameter(EncodedPhenotypeType parameterType, double encoded_value);

#endif