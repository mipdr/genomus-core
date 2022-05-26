#include <iostream>
#include <functional>
#include <cmath>
#include <math.h>

#include "../common/error_handling/error_handling.hpp"
#include "parameter_mapping.hpp"
#include "features.hpp"

static const double E = exp(1.0);
static const double PHI = (1 + sqrt(5)) / 2;
static const double PI = M_PI;

ParameterMapper::ParameterMapper() {
    this -> _name = "DefaultMapper";
    this -> _type = parameterMapper;
    this -> _encode = [](float x){ return x; };
    this -> _decode = [](float y){ return y; };
}

ParameterMapper::ParameterMapper(ParameterMapperInitializer params) {
    this -> _name = params.name;
    this -> _type = parameterMapper;
    this -> _encode = params.decoder;
    this -> _decode = params.decoder;
}

float ParameterMapper::operator>>(const float p) {
    return this -> _encode(p);
}

float ParameterMapper::operator<<(const float p) {
    return this -> _decode(p);
}

std::string ParameterMapper::getName() { return this -> _name; }
FeatureType ParameterMapper::getType() { return this -> _type; }

ParameterMapper ParamF = ParameterMapper({
    .name = "ParamF",
    .encoder = [](float a){ return a; },
    .decoder = [](float a){ return a; }
});

ParameterMapper DurationF = ParameterMapper({
    .name = "DurationF",
    .encoder = [](float p){ return pow(10 * p - 6, 2); },
    .decoder = [](float s){ return (log(s) + 6 * log(2)) / (10 * log(2)); }
});

ParameterMapper NoteValueF = ParameterMapper({
    .name = "NoteValueF",
    .encoder = [](float v){ return (log(v) + 8 * log(2)) / 10 * log(2); },
    .decoder = [](float p){ return pow(10 * p - 8, 2); }
});

ParameterMapper MidiPitchF = ParameterMapper({
    .name = "MidiPitchF",
    .encoder = [](float m){ return (m - 12) / 100; },
    .decoder = [](float p){ return 100 * p + 12; }
});

ParameterMapper FrequencyF = ParameterMapper({
    .name = "FrequencyF",
    .encoder = [](float h){ return pow(h / 20000, 1/4); },
    .decoder = [](float p){ return 20000 * pow(p, 4); }
});

ParameterMapper ArticulationF = ParameterMapper({
    .name = "ArticulationF",
    .encoder = [](float a){ return pow(a / 3, 1 / E); },
    .decoder = [](float p){ return exp(3 * p); }
});

ParameterMapper IntensityF = ParameterMapper({
    .name = "IntensityF",
    .encoder = [](float i){ return i / 127; },
    .decoder = [](float p){ return 127 * p; }
});

ParameterMapper GoldenintegerF = ParameterMapper({
    .name = "GoldenintegerF",
    .encoder = [](int z){ return z * PHI - (int)(z * PHI); },
    .decoder = [](int p){ error(INVALID_CALL); return 0; }
});

ParameterMapper QuantizedF = ParameterMapper({
    .name = "QuantizedF",
    .encoder = [](int z){ return round((asin(pow(2 * z - 1, 17.0 / 11)) / PI + 0.5) * 72 - 36); },
    .decoder = [](float p){ 
        static const map<float, int> LookUpTable({
            { 0, -36 },
            { 0.0005, -35 },
            { 0.001, -34 },
            { 0.003, -33 },
            { 0.006, -32 },
            { 0.008, -31 },
            { 0.01, -30 },
            { 0.015, -29 },
            { 0.02, -28 },
            { 0.025, -27 },
            { 0.03, -26 },
            { 0.04, -25 },
            { 0.045, -24 },
            { 0.05, -23 },
            { 0.06, -22 },
            { 0.07, -21 },
            { 0.08, -20 },
            { 0.09, -19 },
            { 0.1, -18 },
            { 0.11, -17 },
            { 0.12, -16 },
            { 0.14, -15 },
            { 0.15, -14 },
            { 0.16, -13 },
            { 0.18, -12 },
            { 0.2, -11 },
            { 0.21, -10 },
            { 0.23, -9 },
            { 0.25, -8 },
            { 0.27, -7 },
            { 0.3, -6 },
            { 0.32, -5 },
            { 0.33, -4 },
            { 0.36, -3 },
            { 0.4, -2 },
            { 0.45, -1 },
            { 0.5, 0 },
            { 0.55, 1 },
            { 0.6, 2 },
            { 0.64, 3 },
            { 0.67, 4 },
            { 0.68, 5 },
            { 0.7, 6 },
            { 0.73, 7 },
            { 0.75, 8 },
            { 0.77, 9 },
            { 0.79, 10 },
            { 0.8, 11 },
            { 0.82, 12 },
            { 0.84, 13 },
            { 0.85, 14 },
            { 0.86, 15 },
            { 0.88, 16 },
            { 0.89, 17 },
            { 0.9, 18 },
            { 0.91, 19 },
            { 0.92, 20 },
            { 0.93, 21 },
            { 0.94, 22 },
            { 0.95, 23 },
            { 0.955, 24 },
            { 0.96, 25 },
            { 0.97, 26 },
            { 0.975, 27 },
            { 0.98, 28 },
            { 0.985, 29 },
            { 0.99, 30 },
            { 0.992, 31 },
            { 0.994, 32 },
            { 0.997, 33 },
            { 0.999, 34 },
            { 0.9995, 35 },
            { 1, 36 },
        });

        auto it = LookUpTable.begin();
        float min_distance = abs(it -> first - p);
        float current_distance;
        it++;
        for (; it != LookUpTable.end(); it++ ) {
            current_distance = abs(it -> first - p);
            if (current_distance > min_distance) {
                break;
            }
            min_distance = current_distance;
        } 
        it--;
        return it -> second;
    }
});