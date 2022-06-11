#include "utils.hpp"

std::string join(std::vector<std::string> arg, std::string separator) {
    std::string result = "";

    for (std::string item: arg) {
        result += item + separator;
    }

    return result.substr(0, result.length() - separator.length());
}