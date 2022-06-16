#include "utils.hpp"
#include <sstream>

std::string join(std::vector<std::string> arg, std::string separator) {
    std::string result = "";

    for (std::string item: arg) {
        result += item + separator;
    }

    return result.substr(0, result.length() - separator.length());
}

std::string repeat(std::string s, int n) {
    std::stringstream result;
 
    for (int i = 0; i < n; i++) result << s;
 
    return result.str();
}

std::string prettyPrint(std::string s) {
    std::stringstream result;
    std::string tab = "    ";
    unsigned int current_indentation = 0;

    std::string pre = "", post = "";

    for (char c: s) {
        if (c == '(') {
            ++current_indentation;
            result << c << '\n' << repeat(tab, current_indentation);
        } else if (c == ')') {
            if (current_indentation > 0) {
                --current_indentation;
            }
            result << '\n' << repeat(tab, current_indentation) << c;
        } else {
            result << c;
        }
    }

    return result.str();
}