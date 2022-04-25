#include "testing_utils.hpp"
#include "error_handling.hpp"

// template<typename T1, typename T2>
// void assert(T1 expression, T2 result) {
void assert(float expression, double result) {
    if (!(expression == result)) 
        error(ASSERTION_FAILURE);
}