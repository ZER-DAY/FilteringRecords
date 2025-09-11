#pragma once
#include <string>
#include <vector>

/*
 * Enum: RuleType
 * --------------
 * Defines the types of classification rules supported by the program.
 *
 * Possible values:
 *   - HAS_PROPERTY    : checks if a record has a specific property.
 *   - PROPERTY_SIZE   : checks if a property has a specific number of values.
 *   - CONTAINS_VALUE  : checks if a property contains a specific integer value.
 *   - EQUALS_EXACTLY  : checks if a property exactly matches a list of values.
 */
enum RuleType {
    HAS_PROPERTY,
    PROPERTY_SIZE,
    CONTAINS_VALUE,
    EQUALS_EXACTLY
};

// Single rule (feature) of a class.
struct Rule {
    RuleType type{};
    std::string propertyName;
    int expectedSize = 0;                 // for PROPERTY_SIZE
    int expectedValue = 0;                // for CONTAINS_VALUE
    std::vector<int> expectedExactValues; // for EQUALS_EXACTLY
};
