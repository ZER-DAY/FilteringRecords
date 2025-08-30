#pragma once
#include <string>
#include <vector>
#include "Record.h"
#include "ClassRule.h"

// Input file parser (see Appendix B in the spec).
// Format: records (lines) -> blank line -> classes with rules (lines).
namespace Parser {
    // Parses the file at 'path' and fills 'outRecords' and 'outClassRules'.
    // Returns false and sets 'errMsg' on error.
    bool parseFile(const std::string& path,
        std::vector<Record>& outRecords,
        std::vector<ClassRule>& outClassRules,
        std::string& errMsg);
}
