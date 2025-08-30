#include "Parser.h"
#include <cctype>
#include <string>

// Local helpers live in an anonymous namespace.
namespace {
    // Trim whitespace from both ends.
    std::string trim(const std::string& s) {
        size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return "";
        size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }
}
