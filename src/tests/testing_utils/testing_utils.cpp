#include "testing_utils.hpp"
#include "error_handling.hpp"

template<typename T1, typename T2>
void assert(T1 const& expression, T2 const& result) {
    if (!(expression == result)) 
        error(ASSERTION_FAILURE);
}