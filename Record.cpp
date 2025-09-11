/*
 * File: Record.cpp
 * ----------------
 * Implements the methods of the Record structure.
 */

#include "Record.h"

 /*
  * Method: getProperty
  * -------------------
  * Retrieves a property from the record by its name.
  *
  * Parameters:
  *   - propName : the name of the property to look for.
  *
  * Returns:
  *   - pointer to the Property if found,
  *   - nullptr if the property does not exist.
  */
const Property* Record::getProperty(const std::string& propName) const {
    auto it = properties.find(propName);
    return (it == properties.end()) ? nullptr : &it->second;
}
