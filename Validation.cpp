#include "Validation.h"
#include <set>

/*
 * Function: validate_records
 * --------------------------
 * Validates all records to ensure they conform to the specification.
 *
 * Checks performed:
 *   - At least one record must exist.
 *   - Each record must have at least one property.
 *   - No duplicate property names are allowed within the same record.
 *
 * Returns:
 *   DataCheckResult with isCorrect = true if valid,
 *   otherwise false with the corresponding error reason.
 */
DataCheckResult validate_records(const std::vector<Record>& records) {
    // No records found
    if (records.empty())
        return { false, "No records found in input file" };

    for (const auto& rec : records) {
        // Record must have at least one property
        if (rec.properties.empty())
            return { false, "No properties defined for record: " + rec.name };

        // Check for duplicate property names inside the same record
        std::set<std::string> seen;
        for (const auto& kv : rec.properties) {
            if (seen.count(kv.first))
                return { false, "Duplicate property name in record: " + rec.name };
            seen.insert(kv.first);
        }
    }
    return { true, "" }; // Valid records
}

/*
 * Function: validate_classes
 * --------------------------
 * Validates all class rules to ensure they conform to the specification.
 *
 * Checks performed:
 *   - At least one class must exist.
 *   - Each class must contain at least one rule.
 *
 * Returns:
 *   DataCheckResult with isCorrect = true if valid,
 *   otherwise false with the corresponding error reason.
 */
DataCheckResult validate_classes(const std::vector<ClassRule>& classes) {
    // No classes or rules found
    if (classes.empty())
        return { false, "No classes or rules found in input file" };

    for (const auto& cl : classes) {
        // Each class must contain at least one rule
        if (cl.rules.empty())
            return { false, "Class " + cl.className + " has no rules" };
    }
    return { true, "" }; // Valid classes
}
