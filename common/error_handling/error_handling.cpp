#include <iostream>
#include <string>
#include <stdexcept>

#include "error_handling.hpp"

using namespace std;

const map<ErrorCode, string> ErrorMessages({
    { NOT_IMPLEMENTED, "not implemented" },
    { INVALID_CALL, "call not valid" },
    { ASSERTION_FAILURE, "assertion failure error" },
    { DEFAULT_ERROR, "default error" },
});

void error(ErrorCode errorCode, string customMessage) {
    string errorMessage = ErrorMessages.find(errorCode) -> second;
    // cout << "Error was thrown: " << errorMessage << endl;
    // if (customMessage != "") {
    //     cout << "Message: " << customMessage << endl;
    // }
    throw runtime_error(errorMessage);
}