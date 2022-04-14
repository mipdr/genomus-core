#include "parameter_mapping.hpp"

using namespace ParameterMapping;

float ParameterMapper::_encode(float generic_value) const {
    return generic_value;
}

float ParameterMapper::_decode(float normalized_value) const{
    return normalized_value;
}

const float ParameterMapper::operator>>(const float p) const {
    return this -> _encode(p);
}

const float ParameterMapper::operator<<(const float p) const {
    return this -> _decode(p);
}