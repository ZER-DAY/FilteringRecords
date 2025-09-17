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

// ANSI Colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

void print_help() {
    cout << CYAN << "=====================================\n";
    cout << "   RecordClassifier - Help Guide\n";
    cout << "=====================================" << RESET << "\n\n";

    cout << YELLOW << "Required Files:" << RESET << "\n";
    cout << "  1. items.txt  → list of records\n";
    cout << "  2. rules.txt  → list of classification rules\n";
    cout << "  3. output.txt → results will be saved here\n\n";

    cout << YELLOW << "items.txt format:" << RESET << "\n";
    cout << "  <RecordName>: <property1> = [values], <property2> = [values]\n";
    cout << "  Example:\n";
    cout << "    Car: color = [2], doors = [4], engine = [2000]\n\n";

    cout << YELLOW << "rules.txt format:" << RESET << "\n";
    cout << "  <ClassName>: <rule description>\n";
    cout << "  Supported rules:\n";
    cout << "    - has property \"name\"\n";
    cout << "    - property \"name\" has N values\n";
    cout << "    - property \"name\" contains value X\n";
    cout << "    - property \"name\" = [a, b, c]\n\n";

    cout << GREEN << "Example Run:" << RESET << "\n";
    cout << "  Place items.txt and rules.txt in the same folder,\n";
    cout << "  run the program, and results will appear in output.txt\n";
}

int main() {
    cout << CYAN << "=====================================\n";
    cout << "     RecordClassifier - v2.1\n";
    cout << "   Items + Rules → Output\n";
    cout << "=====================================" << RESET << "\n";

    // Ask if user wants help
    cout << YELLOW << "\nPress 'h' for help, or Enter to continue: " << RESET;
    string userChoice;
    getline(cin, userChoice);
    if (userChoice == "h" || userChoice == "H") {
        print_help();
        system("pause");
        return 0;
    }

    // Ask user for filenames (default if empty)
    string itemsFile, rulesFile, outputFile;

    cout << YELLOW << "\nEnter items file name [default: items.txt]: " << RESET;
    getline(cin, itemsFile);
    if (itemsFile.empty()) itemsFile = "items.txt";

    cout << YELLOW << "Enter rules file name [default: rules.txt]: " << RESET;
    getline(cin, rulesFile);
    if (rulesFile.empty()) rulesFile = "rules.txt";

    cout << YELLOW << "Enter output file name [default: output.txt]: " << RESET;
    getline(cin, outputFile);
    if (outputFile.empty()) outputFile = "output.txt";

    // Open items file
    ifstream finItems(itemsFile);
    if (!finItems) {
        cerr << RED << "\n[ERROR] Cannot open file: " << itemsFile << RESET << "\n";
        system("pause");
        return 1;
    }

    // Open rules file
    ifstream finRules(rulesFile);
    if (!finRules) {
        cerr << RED << "\n[ERROR] Cannot open file: " << rulesFile << RESET << "\n";
        system("pause");
        return 1;
    }

    vector<Record> records;
    vector<ClassRule> classes;
    string line;

    // Read items
    cout << YELLOW << "\n[INFO] Reading records from " << itemsFile << "..." << RESET << "\n";
    while (getline(finItems, line)) {
        string clean = trim(line);
        if (clean.empty()) continue;
        Record r;
        if (!parse_record_line(clean, r)) {
            cerr << RED << "[ERROR] Syntax error in record: " << clean << RESET << "\n";
            system("pause");
            return 1;
        }
        records.push_back(r);
    }

    // Read rules
    cout << YELLOW << "[INFO] Reading rules from " << rulesFile << "..." << RESET << "\n";
    while (getline(finRules, line)) {
        string clean = trim(line);
        if (clean.empty()) continue;
        ClassRule cr;
        if (!parse_class_line(clean, cr)) {
            cerr << RED << "[ERROR] Syntax error in rule: " << clean << RESET << "\n";
            system("pause");
            return 1;
        }
        classes.push_back(cr);
    }

    // Validation
    DataCheckResult recCheck = validate_records(records);
    if (!recCheck.isCorrect) {
        cerr << RED << "[ERROR] " << recCheck.reason << RESET << "\n";
        system("pause");
        return 1;
    }

    DataCheckResult clsCheck = validate_classes(classes);
    if (!clsCheck.isCorrect) {
        cerr << RED << "[ERROR] " << clsCheck.reason << RESET << "\n";
        system("pause");
        return 1;
    }

    cout << GREEN << "[OK] Validation successful. Starting classification..." << RESET << "\n";

    // Classification
    auto result = classify(records, classes);

    // Output results to file and console
    ofstream fout(outputFile);
    if (!fout) {
        cerr << RED << "[ERROR] Cannot create file: " << outputFile << RESET << "\n";
        system("pause");
        return 1;
    }

    cout << CYAN << "\n--- Classification Results ---" << RESET << "\n";
    for (auto& c : classes) {
        fout << c.className << ": ";
        cout << c.className << ": ";
        if (result[c.className].empty()) {
            fout << "-\n";
            cout << RED << "-" << RESET << "\n";
        }
        else {
            for (size_t i = 0; i < result[c.className].size(); i++) {
                fout << result[c.className][i];
                cout << GREEN << result[c.className][i] << RESET;
                if (i + 1 < result[c.className].size()) {
                    fout << ", ";
                    cout << ", ";
                }
            }
            fout << "\n";
            cout << RESET << "\n";
        }
    }

    cout << CYAN << "-------------------------------" << RESET << "\n";
    cout << GREEN << "[OK] Results saved to " << outputFile << RESET << "\n";

    system("pause");
    return 0;
}




