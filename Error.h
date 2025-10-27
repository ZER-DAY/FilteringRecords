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
    INVALID_NUMERIC_VALUE,
    NO_RECORDS,
    NO_PROPERTIES_DEFINED,
    TYPE_MISMATCH,
    NO_CLASSES_OR_RULES,
    SYNTAX_ERROR_IN_CLASS_RULE,
    INVALID_RULE_FORMAT,
    VALUE_COUNT_ZERO,
    RULE_BODY_EMPTY,
    INVALID_VALUE_IN_CONTAINS_VALUE
};

/*
 * Struct: Error
 * -------------
 * Describes a specific error with code, ErrorCode, and its source (e.g., Parser or Validator).
 */
struct Error {
    ErrorCode code;      // Type of the error
    std::string source;  // Module or function where it occurred

    // Converts the error code to human-readable text
    std::string codeToString(ErrorCode code);  // no longer static

    // Converts full error info to formatted string
    std::string toString() const;

    // Prints formatted error to console
    void print() const;

    // Comparison operator (needed for std::set<Error>)
    bool operator<(const Error& other) const {
        return static_cast<int>(code) < static_cast<int>(other.code);
    }
};

// Optional: allows printing ErrorCode directly to streams
std::ostream& operator<<(std::ostream& os, const ErrorCode& c);
