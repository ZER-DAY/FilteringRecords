#include "Parser.h"
#include <sstream>
#include <cctype>

/*
 * Function: trim
 * --------------
 * Removes leading and trailing whitespace characters from a string.
 */
std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

/*
 * Function: parseIntList
 * ----------------------
 * Parses a list of integers inside square brackets.
 *
 * Example:
 *   "[1, 2, 3]" → {1, 2, 3}
 */
std::vector<int> parseIntList(const std::string& inside) {
    std::vector<int> result;
    std::stringstream ss(inside);
    char c;
    int value;

    ss >> c; // skip '['
    while (ss >> value) {
        result.push_back(value);
        ss >> c; // ',' or ']'
        if (c == ']') break;
    }
    return result;
}

/*
 * Helper: parseOneRulePhrase
 * --------------------------
 * Parses a single rule phrase (e.g., 'has property "coating"').
 *
 * Supported patterns:
 *   - has property "name"
 *   - property "name" has N values
 *   - property "name" contains value X
 *   - property "name" = [a, b, c]
 */
static bool parseOneRulePhrase(const std::string& phrase, Rule& ruleOut) {
    std::string s = trim(phrase);

    // HAS_PROPERTY
    if (s.find("has property") != std::string::npos) {
        size_t q1 = s.find('"');
        size_t q2 = s.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            ruleOut.type = HAS_PROPERTY;
            ruleOut.propertyName = s.substr(q1 + 1, q2 - q1 - 1);
            return true;
        }
    }

    // PROPERTY_SIZE
    if (s.find("has") != std::string::npos && s.find("values") != std::string::npos) {
        size_t q1 = s.find('"');
        size_t q2 = s.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            ruleOut.propertyName = s.substr(q1 + 1, q2 - q1 - 1);
            std::stringstream ss(s.substr(q2 + 1));
            std::string tmp;
            int n;
            ss >> tmp >> n; // skip "has" and read integer
            ruleOut.type = PROPERTY_SIZE;
            ruleOut.expectedSize = n;
            return true;
        }
    }

    // CONTAINS_VALUE
    if (s.find("contains value") != std::string::npos) {
        size_t q1 = s.find('"');
        size_t q2 = s.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            ruleOut.propertyName = s.substr(q1 + 1, q2 - q1 - 1);
            std::stringstream ss(s.substr(q2 + 1));
            std::string tmp1, tmp2;
            int val;
            ss >> tmp1 >> tmp2 >> val; // skip "contains value" and read integer
            ruleOut.type = CONTAINS_VALUE;
            ruleOut.expectedValue = val;
            return true;
        }
    }

    // EQUALS_EXACTLY
    if (s.find('=') != std::string::npos) {
        size_t q1 = s.find('"');
        size_t q2 = s.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            ruleOut.propertyName = s.substr(q1 + 1, q2 - q1 - 1);
            size_t eq = s.find('=');
            std::string right = trim(s.substr(eq + 1));
            ruleOut.expectedExactValues = parseIntList(right);
            ruleOut.type = EQUALS_EXACTLY;
            return true;
        }
    }

    return false;
}

/*
 * Function: parse_record_line
 * ---------------------------
 * Parses a record definition line from the input file.
 *
 * Example:
 *   "Table: color = [1, 4], size = [20, 40], coating = [44]"
 */
bool parse_record_line(const std::string& line, Record& rec) {
    auto pos = line.find(':');
    if (pos == std::string::npos) return false;

    rec.name = trim(line.substr(0, pos));
    std::string right = trim(line.substr(pos + 1));
    if (rec.name.empty()) return false;

    if (right.empty()) return true; // record with no properties

    std::stringstream ss(right);
    std::string token;
    while (std::getline(ss, token, ',')) {
        auto eq = token.find('=');
        if (eq == std::string::npos) return false;

        std::string pname = trim(token.substr(0, eq));
        std::string pvalue = trim(token.substr(eq + 1));
        if (pname.empty()) return false;

        auto values = parseIntList(pvalue);
        Property prop{ pname, values };
        rec.properties[pname] = std::move(prop);
    }

    return true;
}

/*
 * Function: parse_class_line
 * --------------------------
 * Parses a class definition line from the input file.
 *
 * Example:
 *   "Blue: property \"color\" contains value 1"
 */
bool parse_class_line(const std::string& line, ClassRule& cr) {
    auto pos = line.find(':');
    if (pos == std::string::npos) return false;

    cr.className = trim(line.substr(0, pos));
    std::string desc = trim(line.substr(pos + 1));
    if (cr.className.empty() || desc.empty()) return false;

    Rule r;
    if (!parseOneRulePhrase(desc, r)) return false;
    cr.rules.push_back(std::move(r));

    return true;
}
