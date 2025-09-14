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
 * Parses a list of integers inside square brackets or separated by commas/semicolons.
 *
 * Example:
 *   "[1, 2, 3]" → {1, 2, 3}
 */
std::vector<int> parseIntList(const std::string& inside) {
    std::vector<int> vals;
    std::string t = inside;
    for (char& c : t)
        if (c == ',' || c == ';') c = ' ';
    std::stringstream ss(t);
    int x;
    while (ss >> x) vals.push_back(x);
    return vals;
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
    size_t colon = line.find(':');
    if (colon == std::string::npos) return false;

    rec.name = trim(line.substr(0, colon));
    std::string propsPart = trim(line.substr(colon + 1));

    std::vector<std::string> chunks;
    std::string cur;
    int depth = 0;

    // تقسيم الخصائص حسب الفواصل مع مراعاة الأقواس []
    for (char c : propsPart) {
        if (c == '[') depth++;
        if (c == ']') depth--;
        if (c == ',' && depth == 0) {
            chunks.push_back(trim(cur));
            cur.clear();
        }
        else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) chunks.push_back(trim(cur));

    // تحليل كل خاصية
    for (auto& token : chunks) {
        if (token.empty()) continue;

        size_t eq = token.find('=');
        if (eq == std::string::npos) return false;

        std::string pname = trim(token.substr(0, eq));
        std::string val = trim(token.substr(eq + 1));

        if (val.front() == '[' && val.back() == ']') {
            std::string inside = val.substr(1, val.size() - 2);
            Property p{ pname, parseIntList(inside) };
            rec.properties[pname] = p;
        }
        else {
            return false;
        }
    }
    return true;
}

/*
 * Function: parse_class_line
 * --------------------------
 * Parses a class definition line from the input file.
 *
 * Supported patterns:
 *   - has property "name"
 *   - property "name" has N values
 *   - property "name" contains value X
 *   - property "name" = [a, b, c]
 *
 * Example:
 *   "Blue: property \"color\" contains value 1"
 */
bool parse_class_line(const std::string& line, ClassRule& cr) {
    size_t colon = line.find(':');
    if (colon == std::string::npos) return false;

    cr.className = trim(line.substr(0, colon));
    std::string desc = trim(line.substr(colon + 1));

    Rule r;

    // HAS_PROPERTY
    if (desc.find("has property") != std::string::npos || desc.find("есть свойство") != std::string::npos) {
        r.type = HAS_PROPERTY;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);
    }
    // PROPERTY_SIZE
    else if (desc.find("has") != std::string::npos && desc.find("values") != std::string::npos) {
        r.type = PROPERTY_SIZE;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);

        // extract first number (expected size)
        for (size_t i = 0; i < desc.size(); i++) {
            if (isdigit(desc[i])) {
                r.expectedSize = stoi(desc.substr(i));
                break;
            }
        }
    }
    // CONTAINS_VALUE
    else if (desc.find("contains value") != std::string::npos || desc.find("содержит значение") != std::string::npos) {
        r.type = CONTAINS_VALUE;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);

        // extract first number (expected value)
        for (size_t i = 0; i < desc.size(); i++) {
            if (isdigit(desc[i])) {
                r.expectedValue = stoi(desc.substr(i));
                break;
            }
        }
    }
    // EQUALS_EXACTLY
    else if (desc.find("=") != std::string::npos && desc.find("[") != std::string::npos) {
        r.type = EQUALS_EXACTLY;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);
        size_t lb = desc.find("["), rb = desc.find("]");
        std::string inside = desc.substr(lb + 1, rb - lb - 1);
        r.expectedExactValues = parseIntList(inside);
    }
    else {
        return false;
    }

    cr.rules.push_back(r);
    return true;
}
