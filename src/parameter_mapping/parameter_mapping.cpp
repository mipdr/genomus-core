#include "parameter_mapping.hpp"

using namespace ParameterMapping;

float ParameterMapper::operator>>(float p) {
    return this -> _encode(p);
}

float ParameterMapper::operator<<(float p) {
    return this -> _decode(p);
}