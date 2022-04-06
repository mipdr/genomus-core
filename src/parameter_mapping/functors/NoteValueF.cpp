#include "parameter_mapping.h"
#include <cmath>

class NoteValueF : ParameterMapper {
    private:
        float _encode(float p) {
            return 2 ^ (10 * generic_value - 8);
        }
        float _decode(float v) {
            return (log(v) + 8 * log(2)) / 10 * log(2);
        }
}