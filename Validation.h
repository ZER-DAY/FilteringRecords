#pragma once
#include <vector>
#include "Record.h"
#include "Rule.h"
#include "DataCheckResult.h"

DataCheckResult validate_records(const std::vector<Record>& records);
DataCheckResult validate_classes(const std::vector<ClassRule>& classes);
