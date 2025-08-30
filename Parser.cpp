#include "Parser.h"
#include <string>
#include <vector>
#include <cctype>

// Local helpers live in an anonymous namespace.
namespace {
    // Trim whitespace from both ends.
    std::string trim(const std::string& s) {
        size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return "";
        size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    // Check if a string is whitespace-only.
    bool isBlank(const std::string& s) {
        for (char c : s) if (!std::isspace((unsigned char)c)) return false;
        return true;
    }
}

// Minimal stub to keep project compiling while we add features step by step.
namespace Parser {
    bool parseFile(const std::string&,
        std::vector<Record>&,
        std::vector<ClassRule>&,
        std::string& errMsg) {
        errMsg = "parser not implemented yet";
        return false;
    }
}
