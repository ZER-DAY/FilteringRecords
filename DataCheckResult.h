#pragma once
#include <string>

// Result of input validation.
struct DataCheckResult {
    bool isCorrect = true;   // overall validity flag
    std::string reason;      // reason if invalid
};
