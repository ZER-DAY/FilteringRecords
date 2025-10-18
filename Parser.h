#pragma once
#include <string>
#include <vector>
#include "Record.h"
#include "Rule.h"
#include "Error.h"
#include <set>

/*
 * Function: trim
 * --------------
 * Removes leading and trailing whitespace characters from a string.
 */
std::string trim(const std::string& s);

/*
 * Function: parseIntList
 * ----------------------
 * Parses a list of integers inside square brackets.
 */
std::vector<int> parseIntList(const std::string& inside);

/*
 * Function: parse_record_line
 * ---------------------------
 * Parses a record definition line from the input file.
 * Example:
 *   "Table: color = [1, 4], size = [20, 40]"
 *
 * Now includes error tracking via std::set<Error>& errors.
 */
bool parse_record_line(const std::string& line, Record& rec, std::set<Error>& errors);

/*
 * Function: parse_class_line
 * --------------------------
 * Parses a class rule definition line from the input file.
 *
 * Example:
 *   "Blue: property \"color\" contains value 1"
 *   → ClassRule { className = "Blue", rules = { ... } }
 *
 * Returns:
 *   true  if the line is valid and parsed successfully,
 *   false otherwise.
 */
bool parse_class_line(const std::string& line, ClassRule& cr, std::set<Error>& errors);