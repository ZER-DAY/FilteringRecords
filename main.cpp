#include <iostream>
#include <fstream>
#include <vector>
#include "Record.h"
#include "Rule.h"
#include "Parser.h"
#include "Classifier.h"
#include "Validation.h"

using namespace std;

int main() {
    ifstream fin("input.txt");
    if (!fin) {
        cerr << "Cannot open input.txt\n";
        return 1;
    }

    vector<Record> records;
    vector<ClassRule> classes;
    string line;
    bool readingRecords = true;

    while (getline(fin, line)) {
        string clean = trim(line);
        if (clean.empty()) {
            readingRecords = false;
            continue;
        }

        if (readingRecords) {
            Record r;
            if (parse_record_line(clean, r)) records.push_back(r);
        }
        else {
            ClassRule cr;
            if (parse_class_line(clean, cr)) classes.push_back(cr);
        }
    }

    // Validation checks
    DataCheckResult recCheck = validate_records(records);
    if (!recCheck.isCorrect) {
        cerr << recCheck.reason << "\n";
        return 1;
    }

    DataCheckResult clsCheck = validate_classes(classes);
    if (!clsCheck.isCorrect) {
        cerr << clsCheck.reason << "\n";
        return 1;
    }

    auto result = classify(records, classes);

    for (auto& c : classes) {
        cout << c.className << ": ";
        if (result[c.className].empty()) cout << "-\n";
        else {
            for (size_t i = 0; i < result[c.className].size(); i++) {
                cout << result[c.className][i];
                if (i + 1 < result[c.className].size()) cout << ", ";
            }
            cout << "\n";
        }
    }
}
