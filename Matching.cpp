#include "Matching.h"
#include <algorithm>

namespace Matching {

    // Check if a single record matches a single rule
    bool match_rule(const Record& record, const Rule& rule) {
        auto it = record.properties.find(rule.propertyName);

        switch (rule.type) {
        case RuleType::HAS_PROPERTY:
            // Record must contain the property
            return it != record.properties.end();

        case RuleType::PROPERTY_SIZE:
            // Property must exist and have expected number of values
            return it != record.properties.end() &&
                static_cast<int>(it->second.values.size()) == rule.expectedSize;

        case RuleType::CONTAINS_VALUE:
            // Property must contain a specific value
            if (it == record.properties.end()) return false;
            return std::find(it->second.values.begin(),
                it->second.values.end(),
                rule.expectedValue) != it->second.values.end();

        case RuleType::EQUALS_EXACTLY:
            // Property values must exactly equal expected array
            if (it == record.properties.end()) return false;
            return it->second.values == rule.expectedExactValues;
        }
        return false;
    }
}
