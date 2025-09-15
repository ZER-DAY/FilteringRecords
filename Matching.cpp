#include "Matching.h"

/*
 * Function: match_rule
 * --------------------
 * Evaluates a single rule against a record.
 */
bool match_rule(const Record& record, const Rule& rule) {
    auto it = record.properties.find(rule.propertyName);
    if (it == record.properties.end()) {
        if (rule.type == HAS_PROPERTY) return false;
        return false;
    }
    const Property& prop = it->second;

    switch (rule.type) {
    case HAS_PROPERTY:
        return true;

    case PROPERTY_SIZE:
        return (int)prop.values.size() == rule.expectedSize;

    case CONTAINS_VALUE:
        for (int v : prop.values)
            if (v == rule.expectedValue) return true;
        return false;

    case EQUALS_EXACTLY:
        return prop.values == rule.expectedExactValues;

    default:
        return false;
    }
}

/*
 * Function: match_all_rules
 * -------------------------
 * Returns true if the record satisfies all rules of a given class.
 */
bool match_all_rules(const Record& record, const ClassRule& classRule) {
    for (const auto& r : classRule.rules) {
        if (!match_rule(record, r))
            return false;
    }
    return true;
}
