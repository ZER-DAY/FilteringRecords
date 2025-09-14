#pragma once
#include <map>
#include <string>
#include <vector>
#include "Record.h"
#include "Rule.h"

/*
 * Function: classify
 * ------------------
 * Classifies all records into their respective classes based on rules.
 *
 * Parameters:
 *   - records    : list of records to classify
 *   - classRules : list of class definitions with rules
 *
 * Returns:
 *   A map where:
 *     - key   = class name
 *     - value = list of record names that belong to this class
 */
std::map<std::string, std::vector<std::string>>
classify(const std::vector<Record>& records, const std::vector<ClassRule>& classRules);
