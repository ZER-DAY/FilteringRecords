#pragma once
#include <vector>
#include "Record.h"
#include "ClassRule.h"
#include "DataCheckResult.h"

namespace Validation {
    DataCheckResult validate(const std::vector<Record>& records,
        const std::vector<ClassRule>& classRules);
}
