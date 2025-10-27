#include "Error.h"
#include <iostream>
using namespace std;

/*
 * Converts ErrorCode enum values to human-readable strings.
 * (Now a non-static member function)
 */
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

/*
 * Combines error details into a formatted string.
 */
string Error::toString() const {
    return "[" + const_cast<Error*>(this)->codeToString(code) + "] (Source: " + source + ")";
}

/*
 * Prints formatted error to the standard error output.
 */
void Error::print() const {
    cerr << "[ERROR: "
        << const_cast<Error*>(this)->codeToString(code)
        << "] — Source: " << source << endl;
}

/*
 * Allows streaming ErrorCode directly into output streams.
 * (Since codeToString is not static anymore, we must call it via a temp object)
 */
ostream& operator<<(ostream& os, const ErrorCode& c) {
    Error temp;
    os << temp.codeToString(c);
    return os;
}
