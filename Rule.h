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

/*
 * Structure: Rule
 * ---------------
 * Represents a single classification rule.
 *
 * Fields:
 *   - type                : the type of rule (from RuleType).
 *   - propertyName        : the property to which this rule applies.
 *   - expectedSize        : expected number of values (used for PROPERTY_SIZE).
 *   - expectedValue       : specific value to check (used for CONTAINS_VALUE).
 *   - expectedExactValues : full list of values to match (used for EQUALS_EXACTLY).
 */
struct Rule {
    RuleType type;                        // Type of the rule
    std::string propertyName;             // Target property name
    int expectedSize = 0;                 // For PROPERTY_SIZE
    int expectedValue = 0;                // For CONTAINS_VALUE
    std::vector<int> expectedExactValues; // For EQUALS_EXACTLY
};

/*
 * Structure: ClassRule
 * --------------------
 * Represents a classification category (class) with a list of rules.
 *
 * Example:
 *   "Blue" class might have one rule:
 *       property "color" contains value 1
 *
 * Fields:
 *   - className : the name of the class (e.g., "Blue", "Voluminous").
 *   - rules     : the list of rules that must all be satisfied
 *                 for a record to belong to this class.
 */
struct ClassRule {
    std::string className;        // Name of the class
    std::vector<Rule> rules;      // Rules defining this class
};
