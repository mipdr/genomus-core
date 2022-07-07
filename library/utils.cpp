#include "utils.hpp"
#include <cstdint>
#include <iostream>
#include <ostream>
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

double roundTo6Decimals(double f) {
    return round(f * 1000000) * 1.0 / 1000000.0;
}

std::map<double, size_t> _normalizedToInteger;

double integerToNormalized(size_t x) {
    const double encoded = roundTo6Decimals(PHI * x - (int)(PHI * x));
    _normalizedToInteger[encoded] = x;
    return encoded;
}

size_t normalizedToInteger(double x) {
    static const size_t max_encodable_integer = 100;
    static size_t max_explored_integer = 0;
    if (_normalizedToInteger.find(x) != _normalizedToInteger.end()) {
        return _normalizedToInteger[x];
    }

    // search for result in interval [0, max_encodable_integer]
    size_t result;
    for (size_t i = max_explored_integer; i < max_encodable_integer; i++) {
        if (integerToNormalized(i) == roundTo6Decimals(x)) {
            max_explored_integer = i;
            return i;
        }
    }

    const auto index = getClosestKey(_normalizedToInteger, x);

    return _normalizedToInteger[index];
}

std::string strip(std::string& str){
    auto start_it = str.begin();
    auto end_it = str.rbegin();
    while (std::isspace(*start_it))
        ++start_it;
    while (std::isspace(*end_it))
        ++end_it;
    return std::string(start_it, end_it.base());
}



////////////////
//            //
//    RNG!    //
//            //
////////////////

// Beware! Fun stuff here

uint32_t mulberry_32_next(uint32_t x) {
    // Code mostly copied from tommyettinger's gist:
    // https://gist.github.com/tommyettinger/46a874533244883189143505d203312c
    uint32_t z = (x += 0x6D2B79F5UL);
    z = (z ^ (z >> 15)) * (z | 1UL);
    z ^= z + (z ^ (z >> 7)) * (z | 61UL);
    return z ^ (z >> 14);
}
uint64_t mulberry_32_max = 4294967296;

RandomGenerator::RandomGenerator() {
    this -> _seed = time(NULL);
    this -> _next = mulberry_32_next;
    this -> _max = mulberry_32_max;
}

void RandomGenerator::seed(size_t s) {
    this -> _seed = s;
}

size_t RandomGenerator::next() {
    size_t next = this -> _next(this -> _seed);
    this -> _seed = next;
    return next;
}

double RandomGenerator::nextDouble() {
    return ((double)this -> next()) / ((double)this -> _max);
}