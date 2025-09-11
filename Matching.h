#pragma once
#include <vector>
#include "Record.h"
#include "Rule.h"
#include "DataCheckResult.h"

/*
 * Function: validate_records
 * --------------------------
 * Validates a list of records to ensure correctness.
 */
DataCheckResult validate_records(const std::vector<Record>& records);

/*
 * Function: validate_classes
 * --------------------------
 * Validates a list of class definitions to ensure correctness.
 *
 * Checks performed:
 *   - The list is not empty.
 *   - Each class has at least one rule.
 *
 * Parameters:
 *   - classes : list of class definitions to validate
 *
 * Returns:
 *   DataCheckResult { isCorrect = true } if valid,
 *   otherwise { isCorrect = false, reason = "...error..." }
 */
DataCheckResult validate_classes(const std::vector<ClassRule>& classes);