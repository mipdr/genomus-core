#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../library/genomus-core.hpp"

#define PROMPT "> "
#define FOLLOW_UP_PROMPT "> ... "

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
    },
    {
        .command = "\\q",
        .description = "Quit",
        .get_output = []() {
            exit(0);
            return "";
        }
    }
});

std::string getInfo(dec_gen_t&& tree) {
    std::stringstream ss;

    ss << "{\n    ";
    ss << "Encoded Genotype:\n\t" << to_string(tree.toNormalizedVector()) << "\n\n    ";
    ss << "Decoded Genotype: " << tree.toString() << "\n\n    ";

    auto encoded_phenotype = tree.evaluate();

    ss << "Encoded Phenotype (human readable): " << encoded_phenotype.toString() << "\n\n    ";
    ss << "Encoded Phenotype (normalized vector): " << to_string(encoded_phenotype.toNormalizedVector()) << "\n"; 
    ss << "}\n";   

    return ss.str();
}

void handleCommand(std::string command) {
    auto it = std::find_if(commands.begin(), commands.end(), [&](const Command& c) { return c.command == command; });
    if (it == commands.end()) {
        std::cout << "Unknown command: " << command << std::endl;
    } else {
        std::cout << it -> get_output() << std::endl;
    }
}

void handleInput(std::string input) {
    input = strip(input);

    if (input == "") {
        return;
    }

    try {
        if (input[0] == '\\') {
            handleCommand(input);
        } else {
            std::cout << getInfo(parseString(input)) << std::endl;
        }
    } catch (std::runtime_error e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}

int main() {
    std::string input;
    std::string line_input;
    bool complete_input;

    init_available_functions();

    std::cout << WELCOME << PROMPT;
    while (true) {
        input = "";
        complete_input = false;
        while (!complete_input) {
            getline(std::cin, line_input, '\n');
            input += line_input;
            complete_input = input[input.size() - 1] == ';';
            if (!complete_input)
                std::cout << FOLLOW_UP_PROMPT;
        }

        if (input != "")
            handleInput(input.substr(0, input.size() - 1));

        std::cout << PROMPT;
    }
    return 0;
}