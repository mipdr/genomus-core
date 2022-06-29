#ifndef __GENOMUS_CORE_UTILS__
#define __GENOMUS_CORE_UTILS__ 

#include <algorithm>
#include <functional>
#include <math.h>
#include <string>
#include <vector>
#include <map>

static const double E = exp(1.0);
static const double PHI = (1 + sqrt(5)) / 2;
static const double PI = M_PI;

std::string join(std::vector<std::string>, std::string separator = ", ");

template<typename T>
std::string join(std::vector<T> v, std::string separator = ", ") {
    std::vector<std::string> string_v({});
    std::for_each(v.begin(), v.end(), [&](T element) { string_v.push_back(std::to_string(element)); });
    return join(string_v, separator);
}

template<typename T>
std::string to_string(std::vector<T> v) {
    return "[" + join(v) + "]";
}

template<typename T, typename K>
void values(std::map<T, K> m, std::vector<K>& v) {
    for(std::pair<T, K> entry: m) {
        v.push_back(entry.second);
    }
}

template<typename T>
bool includes(std::vector<T> v, T elem) {
    return std::any_of(v.begin(), v.end(), [&](T t) { return t == elem; });
}

std::string prettyPrint(std::string s);

uint32_t mulberry_32(uint32_t);

double roundTo6Decimals(double f);

template<typename K, typename T>
T findWithDefault(const std::map<K, T>& m, K k, T def_val) {
    auto it = m.find(k);
    if (it == m.end()) {
        return def_val;
    }
    return it -> second;
}

double integerToNormalized(size_t x);
size_t normalizedToInteger(double x);

// Concatenate vectors
template<typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> result(a);
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

template<typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

std::string strip(std::string&);

template<typename K, typename T>
K getClosestKey(std::map<K, T> m, K k) {
    const K upper = m.upper_bound(k) -> first; 
    const K lower = m.lower_bound(k) -> first; 
    const K upper_dif = upper - k;
    const K lower_dif = k - lower;

    return upper_dif < lower_dif ? upper : lower;
}

template<typename T>
T getClosestValue(std::vector<T> v, T val) {
    sort(v.begin(), v.end());

    auto it = v.begin();
    for (it = v.begin(); it != v.end(); ++it) {
        if (val < *it) {
            break;
        }
    }

    if (it == v.begin()) {
        return v[0];
    }

    if (it == v.end()) {
        it--;
        return *it;
    }

    auto previous = it;
    previous--;

    return ((val - *previous) > (*it - val)) ? *it : *previous;
}

#endif