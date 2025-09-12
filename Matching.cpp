#include "Matching.h"

/*
 * Function: match_rule
 * --------------------
 * Checks if a single record matches a single rule.
 *
 * Supported rule types:
 *   - HAS_PROPERTY    : record must contain the property.
 *   - PROPERTY_SIZE   : property must have exact number of values.
 *   - CONTAINS_VALUE  : property must include a specific value.
 *   - EQUALS_EXACTLY  : property values must match exactly the expected list.
 */
bool match_rule(const Record& record, const Rule& rule) {
    auto it = record.properties.find(rule.propertyName);
    if (it == record.properties.end()) {
        return false; // property not found
    }

    const Property& prop = it->second;

    switch (rule.type) {
    case HAS_PROPERTY:
        return true; // property exists

    case PROPERTY_SIZE:
        return static_cast<int>(prop.values.size()) == rule.expectedSize;

    case CONTAINS_VALUE:
        for (int v : prop.values) {
            if (v == rule.expectedValue) return true;
        }
        return false;

    case EQUALS_EXACTLY:
        return prop.values == rule.expectedExactValues;
    }
    return false;
}
