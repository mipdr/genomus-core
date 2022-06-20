#ifndef __GENOMUS_CORE_UTILS__
#define __GENOMUS_CORE_UTILS__ 

#include <algorithm>
#include <functional>
#include <math.h>
#include <string>
#include <vector>

static const double E = exp(1.0);
static const double PHI = (1 + sqrt(5)) / 2;
static const double PI = M_PI;

std::string join(std::vector<std::string>, std::string separator = ", ");

template<typename T>
std::string join(std::vector<T> v, std::function<std::string(const T&)> toString, std::string separator = ", ") {
    std::vector<std::string> string_v({});
    std::for_each(v.begin(), v.end(), [&](T element) { string_v.push_back(toString(element)); });
    return join(string_v, separator);
}

template<typename T>
bool includes(std::vector<T> v, T elem) {
    return std::any_of(v.begin(), v.end(), [&](T t) { return t == elem; });
}

std::string prettyPrint(std::string s);


uint32_t mulberry_32(uint32_t);

#endif