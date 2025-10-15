#pragma once
#include <string>

/*
 * Class: Error
 * ------------
 * Represents a parsing or validation error.
 * Stores an error code and a human-readable message.
 */
struct Error {
    int code;               // Numeric error code
    std::string message;    // Human-readable error message
    std::string source;     // Where it happened (e.g., "Parser", "Validation")

    bool operator<(const Error& other) const noexcept {
        // Needed for std::set ordering
        if (code != other.code)
            return code < other.code;
        return message < other.message;
    }
};
