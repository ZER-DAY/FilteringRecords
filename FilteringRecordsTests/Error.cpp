#include "../Error.h"
#include <iostream>
using namespace std;

string Error::codeToString(ErrorCode code) {
    switch (code) {
    case ErrorCode::INCORRECT_RULE:        return "INCORRECT_RULE";
    case ErrorCode::INVALID_RECORD:        return "INVALID_RECORD";
    case ErrorCode::DUPLICATE_PROPERTY:    return "DUPLICATE_PROPERTY";
    case ErrorCode::MISSING_QUOTE:         return "MISSING_QUOTE";
    case ErrorCode::UNKNOWN_RULE_TYPE:     return "UNKNOWN_RULE_TYPE";
    case ErrorCode::EMPTY_CLASS_NAME:      return "EMPTY_CLASS_NAME";
    case ErrorCode::EMPTY_RECORD_NAME:     return "EMPTY_RECORD_NAME";
    case ErrorCode::INVALID_NUMERIC_VALUE: return "INVALID_NUMERIC_VALUE";
    default:                               return "UNKNOWN";
    }
}

string Error::toString() const {
    return "[" + codeToString(code) + "] " + message + " (" + source + ")";
}

void Error::print() const {
    cerr << "[ERROR: " << codeToString(code) << "] "
        << message << " — Source: " << source << endl;
}
