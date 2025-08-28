#pragma once
#include <string>
#include <vector>

// Property of a record: name + list of integer values.
struct Property {
    std::string name;
    std::vector<int> values;
};
