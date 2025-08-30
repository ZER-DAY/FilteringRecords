#include "Parser.h"
#include <string>
#include <vector>
#include <cctype>
#include <fstream>

// Local helpers live in an anonymous namespace.
namespace {
    // Trim whitespace from both ends.
    std::string trim(const std::string& s) {
        size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return "";
        size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    bool isBlank(const std::string& s) {
        for (char c : s) if (!std::isspace((unsigned char)c)) return false;
        return true;
    }

    // Split by delimiter ignoring delimiters inside [...] blocks.
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
}

namespace Parser {
    bool parseFile(const std::string& path,
        std::vector<Record>&,
        std::vector<ClassRule>&,
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

        errMsg = "parser: sections not processed yet";
        return false;
    }
}
