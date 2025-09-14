#pragma once
#include <vector>
#include "Record.h"
#include "Rule.h"
#include "DataCheckResult.h"

/*
 * Function: match_rule
 * --------------------
 * Checks if a record satisfies a single rule.
 */
bool match_rule(const Record& record, const Rule& rule);

/*
 * Function: match_all_rules
 * -------------------------
 * Checks if a record satisfies all rules of a class.
 */
bool match_all_rules(const Record& record, const ClassRule& classRule);
