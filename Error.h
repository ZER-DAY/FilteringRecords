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
    UNKNOWN
};

struct Error {
    ErrorCode code = ErrorCode::UNKNOWN;
    std::string message;
    std::string source;

    bool operator<(const Error& other) const noexcept {
        if (code != other.code)
            return static_cast<int>(code) < static_cast<int>(other.code);
        if (message != other.message)
            return message < other.message;
        return source < other.source;
    }

    // Declaration only — implemented in Error.cpp
    static std::string codeToString(ErrorCode code);
    std::string toString() const;
    void print() const;
};
