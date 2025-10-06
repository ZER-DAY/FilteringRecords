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
    if (inside.empty()) return vals;

    std::string t = inside;
    for (char& c : t)
        if (c == ',' || c == ';') c = ' ';

    std::stringstream ss(t);
    std::string token;
    while (ss >> token) {
        try {
            size_t pos = 0;
            int v = std::stoi(token, &pos);
            if (pos != token.size()) return {}; // not fully numeric
            vals.push_back(v);
        }
        catch (...) {
            return {}; // contains non-numeric
        }
    }
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
    if (line.empty()) return false;

    size_t colon = line.find(':');
    if (colon == std::string::npos) return false;

    rec.name = trim(line.substr(0, colon));
    std::string propsPart = trim(line.substr(colon + 1));

    // ✅ no properties after colon
    if (propsPart.empty()) return false;

    std::vector<std::string> chunks;
    std::string cur;
    int depth = 0;

    // split properties by commas, respecting brackets
    for (char c : propsPart) {
        if (c == '[') depth++;
        if (c == ']') depth--;
        if (c == ',' && depth == 0) {
            if (!cur.empty()) {
                chunks.push_back(trim(cur));
                cur.clear();
            }
        }
        else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) chunks.push_back(trim(cur));
    if (chunks.empty()) return false;

    for (auto& token : chunks) {
        if (token.empty()) continue;

        size_t eq = token.find('=');
        if (eq == std::string::npos) return false;

        std::string pname = trim(token.substr(0, eq));
        std::string val = trim(token.substr(eq + 1));

        if (val.size() < 2 || val.front() != '[' || val.back() != ']')
            return false;

        std::string inside = val.substr(1, val.size() - 2);
        Property p{ pname, parseIntList(inside) };

        // ✅ reject duplicate property
        if (rec.properties.count(pname))
            return false;

        // ✅ reject non-numeric values (parseIntList failed)
        if (!inside.empty() && p.values.empty())
            return false;

        rec.properties[pname] = p;
    }

    // ✅ record must contain at least one property
    return !rec.properties.empty();
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

    // ✅ reject empty class name
    if (cr.className.empty())
        return false;

    Rule r;

    // HAS_PROPERTY
    if (desc.find("has property") != std::string::npos || desc.find("есть свойство") != std::string::npos) {
        r.type = HAS_PROPERTY;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) return false;
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);
    }
    // PROPERTY_SIZE
    else if (desc.find("has") != std::string::npos && desc.find("values") != std::string::npos) {
        r.type = PROPERTY_SIZE;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) return false;
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);

        for (size_t i = 0; i < desc.size(); i++)
            if (isdigit((unsigned char)desc[i])) {
                r.expectedSize = stoi(desc.substr(i));
                break;
            }
    }
    // CONTAINS_VALUE
    else if (desc.find("contains value") != std::string::npos || desc.find("содержит значение") != std::string::npos) {
        r.type = CONTAINS_VALUE;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) return false;
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);

        for (size_t i = 0; i < desc.size(); i++)
            if (isdigit((unsigned char)desc[i])) {
                r.expectedValue = stoi(desc.substr(i));
                break;
            }
    }
    // EQUALS_EXACTLY
    else if (desc.find("=") != std::string::npos && desc.find("[") != std::string::npos) {
        r.type = EQUALS_EXACTLY;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) return false;
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);
        size_t lb = desc.find("["), rb = desc.find("]");
        if (lb == std::string::npos || rb == std::string::npos || rb <= lb) return false;
        std::string inside = desc.substr(lb + 1, rb - lb - 1);
        r.expectedExactValues = parseIntList(inside);
    }
    else {
        return false;
    }

    cr.rules.push_back(r);
    return true;
}
