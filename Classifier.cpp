#include "Classifier.h"
#include "Matching.h"

/*
 * Function: classify
 * ------------------
 * Iterates over all classes and records, checking which records
 * satisfy all rules of each class.
 */
std::map<std::string, std::vector<std::string>>
classify(const std::vector<Record>& records, const std::vector<ClassRule>& classRules) {
    std::map<std::string, std::vector<std::string>> result;

    for (const auto& c : classRules) {
        for (const auto& r : records) {
            if (match_all_rules(r, c)) {
                result[c.className].push_back(r.name);
            }
        }
    }

    return result;
}
