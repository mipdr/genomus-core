#include "parameter_mapping.hpp"
#include <cmath>

using namespace ParameterMapping;

// To do: optimization

float NoteValueF::_encode(const float p) const {
  return pow(10 * p - 8, 2);
}

float NoteValueF::_decode(const float v) const {
  return (log(v) + 8 * log(2)) / 10 * log(2);
}