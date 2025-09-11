#include "Parser.h"
#include <sstream>
#include <cctype>

/*
 * Function: trim
 * --------------
 * Removes leading and trailing whitespace characters from a string.
 *
 * Example:
 *   "   hello  " → "hello"
 */
std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    size_t e = s.find_last(" \t\r\n");
    return s.substr(b, e - b + 1);
}

/*
 * Function: parseIntList
 * ----------------------
 * Parses a list of integers inside square brackets.
 *
 * Example:
 *   "[1, 2, 3]" → {1, 2, 3}
 *
 * Returns:
 *   vector<int> with parsed values.
 */
std::vector<int> parseIntList(const std::string& inside) {
    std::vector<int> result;
    std::stringstream ss(inside);
    char c;
    int value;

    // Skip the opening '[' if present
    ss >> c;
    while (ss >> value) {
        result.push_back(value);
        ss >> c; // either ',' or ']'
       
    }

    return result;
}