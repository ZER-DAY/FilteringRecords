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
