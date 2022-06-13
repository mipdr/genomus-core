#include <algorithm>
#include <functional>
#include <string>
#include <vector>

std::string join(std::vector<std::string>, std::string separator = ", ");

template<typename T>
std::string join(std::vector<T> v, std::function<std::string(const T&)> toString, std::string separator = ", ") {
    std::vector<std::string> string_v({});
    std::for_each(v.begin(), v.end(), [&](T element) { string_v.push_back(toString(element)); });
    return join(string_v, separator);
}