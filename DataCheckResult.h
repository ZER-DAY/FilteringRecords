#pragma once
#include <string>

/*
 * Structure: DataCheckResult
 * --------------------------
 * Represents the result of validating input data (records or rules).
 *
 * Fields:
 *   - isCorrect : true if the data is valid, false otherwise.
 *   - reason    : description of the error if the data is invalid.
 *
 * Example:
 *   { true, "" }  means the data is valid.
 *   { false, "Duplicate property name" } → means invalid data with explanation.
 */
struct DataCheckResult {
    bool isCorrect;      // Validation flag
    std::string reason;  // reason if invalid
};
