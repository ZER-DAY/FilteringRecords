#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Record.h"
#include "Rule.h"
#include "Parser.h"
#include "Classifier.h"
#include "Validation.h"

using namespace std;

/*
 * Program: RecordClassifier
 * -------------------------
 * Console program for classifying records based on a set of rules.
 *
 * Steps:
 *   1. Read records and class rules from input file.
 *   2. Validate input data (records and rules).
 *   3. Perform classification.
 *   4. Print results to standard output.
 *
 * Usage:
 *   RecordClassifier.exe input.txt
 * If no file is provided, the program will try to open "input.txt".
 */

int main(int argc, char* argv[]) {
    // Step 1. Determine input file
    string filename = "input.txt";
    if (argc > 1) {
        filename = argv[1];
    }

    ifstream fin(filename);
    if (!fin) {
        cerr << "Cannot open the file: " << filename << "\n";
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
            if (!parse_record_line(clean, r)) {
                cerr << "Syntax error in record definition: " << clean << "\n";
                return 1;
            }
            records.push_back(r);
        }
        else {
            ClassRule cr;
            if (!parse_class_line(clean, cr)) {
                cerr << "Syntax error in class rule definition: " << clean << "\n";
                return 1;
            }
            classes.push_back(cr);
        }
    }

    // Step 2. Validate input
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

    // Step 3. Perform classification
    auto result = classify(records, classes);

    // Step 4. Print results
    for (auto& c : classes) {
        cout << c.className << ": ";
        if (result[c.className].empty()) {
            cout << "-\n";
        }
        else {
            for (size_t i = 0; i < result[c.className].size(); i++) {
                cout << result[c.className][i];
                if (i + 1 < result[c.className].size()) cout << ", ";
            }
            cout << "\n";
        }
    }

    return 0;
}
