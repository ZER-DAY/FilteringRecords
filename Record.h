#pragma once
#include <string>
#include <map>
#include "Property.h"

// Record: unique name + dictionary of properties by name.
struct Record {
    std::string name;
    std::map<std::string, Property> properties;
};