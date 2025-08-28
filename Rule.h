#pragma once
#include <string>
#include <vector>

// Types of classification rules.
enum class RuleType {
    HAS_PROPERTY,    // has property "name"
    PROPERTY_SIZE,   // property "name" has N values
    CONTAINS_VALUE,  // property "name" contains value X
    EQUALS_EXACTLY   // property "name" equals [a, b, c]
};

// Single rule (feature) of a class.
struct Rule {
    RuleType type{};
    std::string propertyName;
    int expectedSize = 0;                 // for PROPERTY_SIZE
    int expectedValue = 0;                // for CONTAINS_VALUE
    std::vector<int> expectedExactValues; // for EQUALS_EXACTLY
};
