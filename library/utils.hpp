#ifndef __GENOMUS_CORE_UTILS__
#define __GENOMUS_CORE_UTILS__ 

#include <algorithm>
#include <functional>
#include <math.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <sstream>

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
    std::stringstream ss;

    ss << "[ ";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i % 4 == 0 && i != 0) ss << "\n\t";
        ss << std::to_string(v[i]) << ", ";
    }

    return ss.str().substr(0, ss.str().size() - 2) + " ]";
}

template<typename T, typename K>
void values(std::map<T, K> m, std::vector<K>& v) {
    for(std::pair<T, K> entry: m) {
        v.push_back(entry.second);
    }
}

template<typename T>
bool includes(const std::vector<T>& v, T elem) {
    return std::any_of(v.begin(), v.end(), [&](T t) { return t == elem; });
}

std::string prettyPrint(std::string s);

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

template<typename T>
std::vector<T>& operator+=(std::vector<T>& a, T b) {
    a.push_back(b);
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
T getClosestValue(std::vector<T> v, T val, bool ignore_actual_closest = false) {
    if (!v.size()) {
        throw new std::runtime_error("Vector must have elements");
    }

    if (ignore_actual_closest && v.size() < 2) {
        throw new std::runtime_error("Vector must have two elements to ignore closest");
    }
    
    sort(v.begin(), v.end());

    auto it = v.begin();
    for (it = v.begin(); it != v.end(); ++it) {
        if (val < *it) {
            break;
        }
    }

    if (it == v.begin()) {
        if (ignore_actual_closest) return v[1];
        return v[0];
    }

    if (it == v.end()) {
        it--;
        if (ignore_actual_closest) it--;
        return *it;
    }

    auto previous = it;
    previous--;

    if (ignore_actual_closest) {
        return ((val - *previous) < (*it - val)) ? *it : *previous;
    }

    return ((val - *previous) > (*it - val)) ? *it : *previous;
}

////////////////
//            //
//    RNG!    //
//            //
////////////////

class RandomGenerator {
    private:
        std::function<size_t(size_t)> _next;
        size_t _seed;
        size_t _max;
    public:
        RandomGenerator();
        // RandomGenerator(size_t, std::function<size_t(size_t)>);
        void seed(size_t);
        size_t next();
        double nextDouble();
};

uint32_t mulberry_32(uint32_t);

#endif