#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../library/genomus-core.hpp"

#define PROMPT "> "

static const std::string WELCOME = 
    "GENOMUS-CORE INTERPRETER\n"
    "Copyright © 2022 Miguel Pedregosa Pérez, José López-Montes\n\n"
    "Type an expression to execute with a semi-colon at the end\n"
    "and see what happens!\n\n";
    // "List of commands: ";

typedef struct {
    std::string command;
    std::string description;
    std::function<std::string()> get_output;
} Command;

static const std::vector<Command> commands({
    {
        .command = "\\list",
        .description = "List available functions",
        .get_output = []() {
            std::vector<std::string> v;
            for_each(available_functions.begin(), available_functions.end(), [&](auto& entry) {
                v.push_back(entry.second.getName());
            });
            return join(v, "\n");
        }
    },
    {
        .command = "\\list_verbose",
        .description = "List available functions with additional information",
        .get_output = []() {
            std::vector<std::string> v;
            for_each(available_functions.begin(), available_functions.end(), [&](auto& entry) {
                v.push_back(entry.second.toString());
            });
            return join(v, "\n");
        }
    }
});

std::string getInfo(dec_gen_t&& tree) {
    std::stringstream ss;

    ss << "{\n\t";
    ss << "Encoded Genotype: " << to_string(tree.toNormalizedVector()) << "\n\n\t";
    ss << "Decoded Genotype: " << tree.toString() << "\n\n\t";

    auto encoded_phenotype = tree.evaluate();

    ss << "Encoded Phenotype (human readable): " << encoded_phenotype.toString() << "\n\n\t";
    ss << "Encoded Phenotype (normalized vector): " << to_string(encoded_phenotype.toNormalizedVector()) << "\n"; 
    ss << "}\n";   

    return ss.str();
}

int main() {
    std::string input;

    init_available_functions();

    std::cout << WELCOME << PROMPT;
    while (true) {
        getline(std::cin, input, ';');
        input = strip(input);

        try {
            if (input[0] == '\\') {
                auto it = std::find_if(commands.begin(), commands.end(), [&](const Command& c) { return c.command == input; });
                if (it == commands.end()) {
                    std::cout << "Unknown command: " << input << std::endl;
                } else {
                    std::cout << it -> get_output() << std::endl;
                }
            } else {
                std::cout << getInfo(parseString(input)) << std::endl;
            }
        } catch (std::runtime_error e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
        std::cout << PROMPT;
    }
    return 0;
}