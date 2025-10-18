#pragma once
#include <string>
#include <iostream>

/*
 * Class: Error
 * ------------
 * Represents a parsing or validation error.
 * Stores an error code and a human-readable message.
 */
enum class ErrorCode {
    INCORRECT_RULE,
    INVALID_RECORD,
    DUPLICATE_PROPERTY,
    MISSING_QUOTE,
    UNKNOWN_RULE_TYPE,
    EMPTY_CLASS_NAME,
    EMPTY_RECORD_NAME,
    INVALID_NUMERIC_VALUE
};

/*
 * Struct: Error
 * -------------
 * Describes a specific error with code, message, and its source (e.g., Parser or Validator).
 */
struct Error {
    ErrorCode code;          // Type of the error
    std::string message;     // Description of the error
    std::string source;      // Module or function where it occurred

    // Converts the error details to human-readable text
    std::string toString() const;

    // Converts ErrorCode enum to string
    static std::string codeToString(ErrorCode code);

    // Prints formatted error to console
    void print() const;

    // Comparison operator (needed for std::set<Error>)
    bool operator<(const Error& other) const {
        return static_cast<int>(code) < static_cast<int>(other.code);
    }
};

// Optional: allows printing ErrorCode directly to streams (cout/cerr)
std::ostream& operator<<(std::ostream& os, const ErrorCode& c);
