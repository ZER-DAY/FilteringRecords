#pragma once
#include "Record.h"
#include "Rule.h"
#include "ClassRule.h"

namespace Matching {
    bool match_rule(const Record& record, const Rule& rule);
    bool match_all_rules(const Record& record, const ClassRule& classRule);
}
