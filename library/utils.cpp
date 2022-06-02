#include "utils.hpp"

string join(vector<string> arg, string separator) {
    string result = "";

    for (string item: arg) {
        result += item + separator;
    }

    return result.substr(0, result.length() - separator.length());
}