#pragma once
#include <string>
#include <vector>
#include <map>

/*
 * Structure: Property
 * -------------------
 * Represents a single property of a record.
 * Example: "color = [1, 2]"
 *
 * Fields:
 *   - name: the name of the property (e.g., "color", "size").
 *   - values: a list of integer values associated with this property.
 */
struct Property {
    std::string name;              // Property name
    std::vector<int> values;       // List of integer values
};

/*
 * Structure: Record
 * -----------------
 * Represents a complete record (e.g., a furniture item).
 * A record consists of a name and a set of properties.
 *
 * Example:
 *   Wardrobe: color = [1, 2], size = [10, 40, 60]
 *
 * Fields:
 *   - name: the unique name of the record.
 *   - properties: a map where each key is the property name
 *                 and the value is a Property structure.
 *
 * Methods:
 *   - getProperty: retrieves a property by name. Returns nullptr if not found.
 */
struct Record {
    std::string name;                                // Record name (e.g., "Wardrobe")
    std::map<std::string, Property> properties;      // Map of property name → Property

    // Retrieve a property by its name (returns nullptr if not found)
    const Property* getProperty(const std::string& propName) const;
};
