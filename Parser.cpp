#include "Parser.h"
#include <string>
#include <vector>
#include <cctype>
#include <fstream>
#include <sstream>

// some helper functions for strings
namespace {
    std::string trim(const std::string& s) {
        size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return "";
        size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    // check if line is only spaces
    bool isBlank(const std::string& s) {
        for (char c : s) if (!std::isspace((unsigned char)c)) return false;
        return true;
    }

    // split by delimiter, but ignore delimiters inside [ ... ]
    std::vector<std::string> splitOutsideBrackets(const std::string& line, char delim) {
        std::vector<std::string> parts;
        std::string cur;
        int depth = 0;
        for (char c : line) {
            if (c == '[') ++depth;
            else if (c == ']') --depth;
            if (c == delim && depth == 0) {
                parts.push_back(trim(cur));
                cur.clear();
            }
            else {
                cur.push_back(c);
            }
        }
        if (!cur.empty()) parts.push_back(trim(cur));
        return parts;
    }

    // turn "[1, 2, 3]" into vector<int> {1,2,3}
    bool parseIntVector(const std::string& in, std::vector<int>& out) {
        out.clear();
        std::string s = trim(in);
        size_t lb = s.find('['), rb = s.rfind(']');
        if (lb == std::string::npos || rb == std::string::npos || rb < lb) return false;
        std::string inside = s.substr(lb + 1, rb - lb - 1);
        auto items = splitOutsideBrackets(inside, ',');
        for (auto& it : items) {
            if (it.empty()) continue;
            try { out.push_back(std::stoi(trim(it))); }
            catch (...) { return false; }
        }
        return true;
    }

    // parse a record line like:
    // "Стол: цвет = [1, 4], размер = [20, 40], покрытие = [44]"
    bool parseRecordLine(const std::string& line, Record& rec) {
        auto pos = line.find(':');
        if (pos == std::string::npos) return false;

        rec.name = trim(line.substr(0, pos));
        std::string right = trim(line.substr(pos + 1));
        if (rec.name.empty()) return false;

        if (right.empty()) return true;

        auto props = splitOutsideBrackets(right, ',');
        for (auto& p : props) {
            auto eq = p.find('=');
            if (eq == std::string::npos) return false;
            std::string pname = trim(p.substr(0, eq));
            std::string pvalue = trim(p.substr(eq + 1));
            if (pname.empty()) return false;

            std::vector<int> values;
            if (!parseIntVector(pvalue, values)) return false;

            Property prop{ pname, values };
            rec.properties[pname] = std::move(prop);
        }
        return true;
    }

    // get property name between quotes, e.g. "color"
    bool extractQuotedProperty(const std::string& s, std::string& propName) {
        size_t q1 = s.find('"');
        size_t q2 = s.find('"', q1 == std::string::npos ? std::string::npos : q1 + 1);
        if (q1 == std::string::npos || q2 == std::string::npos || q2 <= q1 + 1) return false;
        propName = s.substr(q1 + 1, q2 - q1 - 1);
        return true;
    }

    // supports all 4: HAS_PROPERTY, PROPERTY_SIZE, CONTAINS_VALUE, EQUALS_EXACTLY
    bool parseOneRulePhrase(const std::string& phrase, Rule& ruleOut) {
        std::string s = trim(phrase);

        // EQUALS_EXACTLY: property "name" equals [a, b, c]
        {
            size_t eq = s.find('=');
            if (eq != std::string::npos) {
                std::string left = trim(s.substr(0, eq));
                std::string right = trim(s.substr(eq + 1));
                std::string prop;
                if (extractQuotedProperty(left, prop)) {
                    std::vector<int> expected;
                    if (parseIntVector(right, expected)) {
                        ruleOut.type = RuleType::EQUALS_EXACTLY;
                        ruleOut.propertyName = prop;
                        ruleOut.expectedExactValues = std::move(expected);
                        return true;
                    }
                }
            }
        }

        // HAS_PROPERTY
        if (s.find("есть свойство") != std::string::npos) {
            std::string prop;
            if (extractQuotedProperty(s, prop)) {
                ruleOut.type = RuleType::HAS_PROPERTY;
                ruleOut.propertyName = prop;
                return true;
            }
        }

        // PROPERTY_SIZE
        if (s.find("содержит") != std::string::npos &&
            s.find("значен") != std::string::npos &&
            s.find("значение") == std::string::npos) {
            std::string prop;
            if (extractQuotedProperty(s, prop)) {
                std::string tail = s.substr(s.find("содержит") + 8);
                std::stringstream ss(tail);
                int n = 0;
                if (ss >> n) {
                    ruleOut.type = RuleType::PROPERTY_SIZE;
                    ruleOut.propertyName = prop;
                    ruleOut.expectedSize = n;
                    return true;
                }
            }
        }

        // CONTAINS_VALUE
        if (s.find("содержит значение") != std::string::npos) {
            std::string prop;
            if (extractQuotedProperty(s, prop)) {
                std::string tail = s.substr(s.find("содержит значение") + 17);
                std::stringstream ss(tail);
                int x = 0;
                if (ss >> x) {
                    ruleOut.type = RuleType::CONTAINS_VALUE;
                    ruleOut.propertyName = prop;
                    ruleOut.expectedValue = x;
                    return true;
                }
            }
        }

        return false;
    }

    // parse class line like:
    // "Синий: свойство "цвет" содержит значение 1"
    bool parseClassLine(const std::string& line, ClassRule& cr) {
        auto pos = line.find(':');
        if (pos == std::string::npos) return false;

        cr.className = trim(line.substr(0, pos));
        std::string desc = trim(line.substr(pos + 1));
        if (cr.className.empty() || desc.empty()) return false;

        std::vector<std::string> phrases;
        if (desc.find(';') == std::string::npos) {
            phrases.push_back(desc);
        }
        else {
            phrases = splitOutsideBrackets(desc, ';');
        }

        cr.rules.clear();
        for (auto& ph : phrases) {
            if (trim(ph).empty()) continue;
            Rule r;
            if (!parseOneRulePhrase(ph, r)) return false;
            cr.rules.push_back(std::move(r));
        }
        return true;
    }
}

// parseFile: split blocks, parse records, then parse classes
namespace Parser {
    bool parseFile(const std::string& path,
        std::vector<Record>& outRecords,
        std::vector<ClassRule>& outClassRules,
        std::string& errMsg) {
        std::ifstream fin(path);
        if (!fin.is_open()) { errMsg = "Cannot open the file"; return false; }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(fin, line)) {
            if (lines.empty() && line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF) {
                line = line.substr(3);
            }
            lines.push_back(line);
        }

        size_t i = 0;
        // optional header
        if (i < lines.size() && trim(lines[i]) == "Записи:") ++i;

        // collect records until blank or "Классы:"
        std::vector<std::string> recLines;
        for (; i < lines.size(); ++i) {
            std::string t = trim(lines[i]);
            if (t == "Классы:" || isBlank(t)) break;
            recLines.push_back(lines[i]);
        }

        while (i < lines.size() && isBlank(lines[i])) ++i;
        // optional header for classes
        if (i < lines.size() && trim(lines[i]) == "Классы:") ++i;

        // collect class lines (we'll parse later)
        std::vector<std::string> classLines;
        for (; i < lines.size(); ++i) {
            std::string t = trim(lines[i]);
            if (t.empty()) continue;
            classLines.push_back(lines[i]);
        }

        // parse records now
        outRecords.clear();
        for (auto& rl : recLines) {
            Record r;
            if (!parseRecordLine(rl, r)) {
                errMsg = "Invalid record line: " + rl;
                return false;
            }
            outRecords.push_back(std::move(r));
        }

        // parse class rules now
        outClassRules.clear();
        for (auto& cl : classLines) {
            ClassRule cr;
            if (!parseClassLine(cl, cr)) {
                errMsg = "Invalid class/rule line: " + cl;
                return false;
            }
            outClassRules.push_back(std::move(cr));
        }

        errMsg.clear();
        return true;
    }
}
