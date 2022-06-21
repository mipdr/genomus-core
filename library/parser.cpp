#include "parser.hpp"
#include "decoded_genotype.hpp"
#include "encoded_genotype.hpp"
#include "errorCodes.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

size_t countOcurrences(std::string entry, char c) {
    size_t result = 0;
    for (char x : entry) if (x == c) result++;

    return result;
}

bool wellFormedParenthesis(std::string entry) {
    return countOcurrences(entry, '(') == countOcurrences(entry, ')');
}

std::vector<std::string> getTokens(std::string entry) {
    std::string current_token = "";
    std::vector<std::string> tokens;

    for (char c: entry) {
        if (c == '(' || c == ')' || c == ',') {
            if (current_token.size()) tokens.push_back(current_token);
            tokens.push_back(std::string({c}));
            current_token = "";
        } else {
            if (c != ' ' && c != '{' && c != '}') 
                current_token += c;
        }
    }

    return tokens;
}

struct TokenNode {
    std::string token;
    std::vector<size_t> children;
    size_t parent;
    bool root;
};

std::string print(const TokenNode& tn) {
    std::stringstream ss;
    ss << "Token: " << tn.token << '\n';
    ss << "Children: " << join(tn.children) << '\n';
    ss << "Parent: " << tn.parent;

    return ss.str();
}

void printTokenTree(const std::vector<TokenNode>& tree) {
    std::cout << "NODES: [\n";
    int i = 0;
    for (auto node: tree) {
        std::cout << "{ NODE " << i << "\n";
        std::cout << print(node) << "\n},\n";
        i++;
    }
    std::cout << "]\n";
}

bool isTokenNumeric(std::string token) {
    return std::regex_match(token, std::regex("-?[0-9]+([\\.][0-9]+)?"));
}

std::vector<TokenNode> buildTokenTree(std::string entry) {
    if (!wellFormedParenthesis(entry)) throw std::runtime_error(ErrorCodes::BAD_PARSER_ENTRY_BAD_PARENTHESIS);
    auto tokens = getTokens(entry);

    size_t current_parent = 0;
    std::vector<TokenNode> nodes;

    for (auto token: tokens) {
        if (token == "(") {
            current_parent = nodes.size() - 1;
        } else if (token == ")") {
            current_parent = nodes[current_parent].parent;
        } else if (token == ",") {

        } else {
            nodes.push_back({
                .token = token,
                .children = {},
                .parent = current_parent,
                .root = false,
            });

            if (nodes.size() > 1) {
                nodes[current_parent].children.push_back(nodes.size() - 1);
            }
        }
    }

    return nodes;
}

dec_gen_t tokenTreeToGTree(const std::vector<TokenNode>& token_nodes, size_t index = 0) {
    auto token = token_nodes[index].token;
    auto f_it = std::find_if(available_functions.begin(), available_functions.end(), [&token](GTree::GFunction& f) { 
        return f.getName() == token; 
    });
    if (f_it == available_functions.end()) throw std::runtime_error(ErrorCodes::BAD_PARSER_ENTRY_BAD_FUNCTION_NAME + ": " + token);

    if (token_nodes[index].children.size()) {
        auto first_child_token = token_nodes[token_nodes[index].children[0]].token; 
        if (isTokenNumeric(first_child_token)) {
            // Missing check for no siblings and no children
            return (*f_it)(std::stof(first_child_token));
        }
    }

    std::vector<dec_gen_t> children({});

    std::for_each(token_nodes[index].children.begin(), token_nodes[index].children.end(), [&](size_t child_index) {
        children.push_back(tokenTreeToGTree(token_nodes, child_index));
    });

    return (*f_it)(children);
}

dec_gen_t parseString(std::string entry) {
    if (!wellFormedParenthesis(entry)) 
        throw std::runtime_error(ErrorCodes::BAD_PARSER_ENTRY_BAD_PARENTHESIS);

    auto token_tree = buildTokenTree(entry);
    
    return tokenTreeToGTree(token_tree);
}   
