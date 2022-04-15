#include "parameter_mapping.hpp"
#include <functional>
#include <cmath>

ParameterMapper::ParameterMapper() {
    this -> _name = "DefaultMapper";
    this -> _encode = [](float x){ return x; };
    this -> _decode = [](float y){ return y; };
}

ParameterMapper::ParameterMapper(ParameterMapperInitializer params) {
    this -> _name = params.name;
    this -> _encode = params.encoder;
    this -> _decode = params.decoder;
}

const float ParameterMapper::operator>>(const float p) const {
    return this -> _encode(p);
}

const float ParameterMapper::operator<<(const float p) const {
    return this -> _decode(p);
}

ParameterMapper ParamF = ParameterMapper({
    .name = "ParamF",
    .encoder = [](float a){ return a; },
    .decoder = [](float a){ return a; }
});

ParameterMapper DurationF = ParameterMapper({
    .name = "DurationF",
    .encoder = [](float s){ return (log(s) + 6 * log(2)) / (10 * log(2)); },
    .decoder = [](float p){ return pow(10 * p - 6, 2); }
});

ParameterMapper NoteValueF = ParameterMapper({
    .name = "NoteValueF",
    .encoder = [](float p){ return pow(10 * p - 8, 2); },
    .decoder = [](float v){ return (log(v) + 8 * log(2)) / 10 * log(2); }
});