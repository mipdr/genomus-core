#ifndef __GENOMUS_CORE_ERROR_HANDLING_UTILS__
#define __GENOMUS_CORE_ERROR_HANDLING_UTILS__

#include <map>

using namespace std;

enum ErrorCode {
    NOT_IMPLEMENTED,
    INVALID_CALL,
    ASSERTION_FAILURE,
    DEFAULT_ERROR
};

extern const map<ErrorCode, string> ErrorMessages;

void error(ErrorCode errorCode);

#endif