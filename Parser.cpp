#include "Parser.h"
#include "Error.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <set>

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
 *
 * Uses std::set<Error>& errors (no message field anymore)
 */
bool parse_record_line(const std::string& line, Record& rec, std::set<Error>& errors) {
    if (line.empty()) {
        Error e{ ErrorCode::INVALID_RECORD, "Parser" };
        errors.insert(e);
        return false;
    }

    size_t colon = line.find(':');
    if (colon == std::string::npos) {
        Error e{ ErrorCode::INVALID_RECORD, "Parser" };
        errors.insert(e);
        return false;
    }

    rec.name = trim(line.substr(0, colon));
    if (rec.name.empty()) {
        Error e{ ErrorCode::EMPTY_RECORD_NAME, "Parser" };
        errors.insert(e);
        return false;
    }

    std::string propsPart = trim(line.substr(colon + 1));
    if (propsPart.empty()) {
        Error e{ ErrorCode::INVALID_RECORD, "Parser" };
        errors.insert(e);
        return false;
    }

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
    if (chunks.empty()) {
        Error e{ ErrorCode::INVALID_RECORD, "Parser" };
        errors.insert(e);
        return false;
    }

    for (auto& token : chunks) {
        if (token.empty()) continue;

        size_t eq = token.find('=');
        if (eq == std::string::npos) {
            Error e{ ErrorCode::INVALID_RECORD, "Parser" };
            errors.insert(e);
            return false;
        }

        std::string pname = trim(token.substr(0, eq));
        std::transform(pname.begin(), pname.end(), pname.begin(),
            [](unsigned char c) { return std::tolower(c); });

        std::string val = trim(token.substr(eq + 1));
        if (val.size() < 2 || val.front() != '[' || val.back() != ']') {
            Error e{ ErrorCode::INCORRECT_RULE, "Parser" };
            errors.insert(e);
            return false;
        }

        std::string inside = val.substr(1, val.size() - 2);
        Property p{ pname, parseIntList(inside) };

        // reject duplicate property
        if (rec.properties.count(pname)) {
            Error e{ ErrorCode::DUPLICATE_PROPERTY, "Parser" };
            errors.insert(e);
            return false;
        }

        // reject non-numeric values
        if (!inside.empty() && p.values.empty()) {
            Error e{ ErrorCode::INVALID_NUMERIC_VALUE, "Parser" };
            errors.insert(e);
            return false;
        }

        rec.properties[pname] = p;
    }

    if (rec.properties.empty()) {
        Error e{ ErrorCode::INVALID_RECORD, "Parser" };
        errors.insert(e);
        return false;
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
 * Uses std::set<Error>& errors (no message field)
 */
bool parse_class_line(const std::string& line, ClassRule& cr, std::set<Error>& errors) {
    size_t colon = line.find(':');
    if (colon == std::string::npos) {
        Error e{ ErrorCode::INCORRECT_RULE, "Parser" };
        errors.insert(e);
        return false;
    }

    cr.className = trim(line.substr(0, colon));
    std::string desc = trim(line.substr(colon + 1));

    if (cr.className.empty()) {
        Error e{ ErrorCode::EMPTY_CLASS_NAME, "Parser" };
        errors.insert(e);
        return false;
    }

    Rule r;

    // HAS_PROPERTY
    if (desc.find("has property") != std::string::npos || desc.find("есть свойство") != std::string::npos) {
        r.type = HAS_PROPERTY;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) {
            Error e{ ErrorCode::MISSING_QUOTE, "Parser" };
            errors.insert(e);
            return false;
        }
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);
    }
    // PROPERTY_SIZE
    else if (desc.find("has") != std::string::npos && desc.find("values") != std::string::npos) {
        r.type = PROPERTY_SIZE;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) {
            Error e{ ErrorCode::MISSING_QUOTE, "Parser" };
            errors.insert(e);
            return false;
        }
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);

        for (size_t i = 0; i < desc.size(); i++)
            if (isdigit((unsigned char)desc[i])) {
                r.expectedSize = stoi(desc.substr(i));
                break;
            }

        if (r.expectedSize <= 0) {
            Error e{ ErrorCode::INVALID_NUMERIC_VALUE, "Parser" };
            errors.insert(e);
            return false;
        }
    }
    // CONTAINS_VALUE
    else if (desc.find("contains value") != std::string::npos || desc.find("содержит значение") != std::string::npos) {
        r.type = CONTAINS_VALUE;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) {
            Error e{ ErrorCode::MISSING_QUOTE, "Parser" };
            errors.insert(e);
            return false;
        }
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);

        size_t pos = desc.find("value");
        if (pos == std::string::npos) {
            Error e{ ErrorCode::INCORRECT_RULE, "Parser" };
            errors.insert(e);
            return false;
        }

        std::string afterValue = trim(desc.substr(pos + 5));
        if (afterValue.empty()) {
            Error e{ ErrorCode::INVALID_NUMERIC_VALUE, "Parser" };
            errors.insert(e);
            return false;
        }

        for (char c : afterValue)
            if (!isdigit((unsigned char)c) && c != ' ') {
                Error e{ ErrorCode::INVALID_NUMERIC_VALUE, "Parser" };
                errors.insert(e);
                return false;
            }

        r.expectedValue = stoi(afterValue);
    }
    // EQUALS_EXACTLY
    else if (desc.find("=") != std::string::npos && desc.find("[") != std::string::npos) {
        r.type = EQUALS_EXACTLY;
        size_t q1 = desc.find("\""), q2 = desc.find_last_of("\"");
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1) {
            Error e{ ErrorCode::MISSING_QUOTE, "Parser" };
            errors.insert(e);
            return false;
        }
        r.propertyName = desc.substr(q1 + 1, q2 - q1 - 1);
        size_t lb = desc.find("["), rb = desc.find("]");
        if (lb == std::string::npos || rb == std::string::npos || rb <= lb) {
            Error e{ ErrorCode::INCORRECT_RULE, "Parser" };
            errors.insert(e);
            return false;
        }
        std::string inside = desc.substr(lb + 1, rb - lb - 1);
        r.expectedExactValues = parseIntList(inside);
        if (r.expectedExactValues.empty()) {
            Error e{ ErrorCode::INVALID_NUMERIC_VALUE, "Parser" };
            errors.insert(e);
            return false;
        }
    }
    else {
        Error e{ ErrorCode::UNKNOWN_RULE_TYPE, "Parser" };
        errors.insert(e);
        return false;
    }

    cr.rules.push_back(r);
    return true;
}
