#include "Validation.h"
#include <unordered_set>

// Error messages from specification
static const char* ERR_NO_RECORDS = "No records found in input file";
static const char* ERR_NO_CLASSES_OR_RULES = "No classes or rules found in input file";
static const char* ERR_EMPTY_PROPS = "No properties defined for record";
static const char* ERR_DUPLICATE_PROPERTY = "Duplicate property name detected";

namespace Validation {

    // Validate records and class rules according to rules in Appendix A
    DataCheckResult validate(const std::vector<Record>& records,
        const std::vector<ClassRule>& classRules) {
        DataCheckResult r;

        // No records found
        if (records.empty()) { r.isCorrect = false; r.reason = ERR_NO_RECORDS; return r; }
        // No classes or rules found
        if (classRules.empty()) { r.isCorrect = false; r.reason = ERR_NO_CLASSES_OR_RULES; return r; }

        // Validate each record
        for (const auto& rec : records) {
            // Record must have at least one property
            if (rec.properties.empty()) {
                r.isCorrect = false; r.reason = ERR_EMPTY_PROPS; return r;
            }
            // Check for duplicate property names inside the same record
            std::unordered_set<std::string> seen;
            for (const auto& kv : rec.properties) {
                if (!seen.insert(kv.first).second) {
                    r.isCorrect = false; r.reason = ERR_DUPLICATE_PROPERTY; return r;
                }
            }
        }

        // Validate each class rule (must contain at least one rule)
        for (const auto& cr : classRules) {
            if (cr.rules.empty()) {
                r.isCorrect = false; r.reason = ERR_NO_CLASSES_OR_RULES; return r;
            }
        }
        return r; // Valid input
    }
}
