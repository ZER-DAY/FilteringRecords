#pragma once
#include <string>
#include <vector>
#include "Rule.h"

// Class definition: class name + list of rules.
struct ClassRule {
    std::string className;
    std::vector<Rule> rules;
};
