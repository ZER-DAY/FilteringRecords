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

    cout << YELLOW << "Input File Format:" << RESET << "\n";
    cout << "  Records section:\n";
    cout << "    <RecordName>: <property1> = [values], <property2> = [values]\n";
    cout << "  Example:\n";
    cout << "    Car: color = [2], doors = [4], engine = [2000]\n\n";

    cout << "  Class rules section (after empty line):\n";
    cout << "    Supported patterns:\n";
    cout << "      - has property \"name\"\n";
    cout << "      - property \"name\" has N values\n";
    cout << "      - property \"name\" contains value X\n";
    cout << "      - property \"name\" = [a, b, c]\n\n";

    cout << GREEN << "Example Run:" << RESET << "\n";
    cout << "  Provide an input file when asked (e.g., input.txt)\n";
}

int main() {
    cout << CYAN << "=====================================\n";
    cout << "     RecordClassifier - v1.1\n";
    cout << "   Classification Based on Rules\n";
    cout << "=====================================" << RESET << "\n";

    // Ask user for input
    string filename;
    cout << YELLOW << "\nEnter input file name (or -h for help): " << RESET;
    cin >> filename;

    if (filename == "-h" || filename == "--help") {
        print_help();
        system("pause");
        return 0;
    }

    ifstream fin(filename);
    if (!fin) {
        cerr << RED << "\n[ERROR] Cannot open the file: " << filename << RESET << "\n";
        system("pause");
        return 1;
    }

    vector<Record> records;
    vector<ClassRule> classes;
    string line;
    bool readingRecords = true;

    cout << YELLOW << "\n[INFO] Reading input file: " << filename << "..." << RESET << "\n";

    while (getline(fin, line)) {
        string clean = trim(line);
        if (clean.empty()) {
            readingRecords = false;
            continue;
        }

        if (readingRecords) {
            Record r;
            if (!parse_record_line(clean, r)) {
                cerr << RED << "\n[ERROR] Syntax error in record definition:\n  "
                    << clean << RESET << "\n";
                system("pause");
                return 1;
            }
            records.push_back(r);
        }
        else {
            ClassRule cr;
            if (!parse_class_line(clean, cr)) {
                cerr << RED << "\n[ERROR] Syntax error in class rule definition:\n  "
                    << clean << RESET << "\n";
                system("pause");
                return 1;
            }
            classes.push_back(cr);
        }
    }

    // Validation
    DataCheckResult recCheck = validate_records(records);
    if (!recCheck.isCorrect) {
        cerr << RED << "\n[ERROR] " << recCheck.reason << RESET << "\n";
        system("pause");
        return 1;
    }

    DataCheckResult clsCheck = validate_classes(classes);
    if (!clsCheck.isCorrect) {
        cerr << RED << "\n[ERROR] " << clsCheck.reason << RESET << "\n";
        system("pause");
        return 1;
    }

    cout << GREEN << "[OK] Validation successful. Starting classification..." << RESET << "\n";

    // Classification
    auto result = classify(records, classes);

    // Output results
    cout << CYAN << "\n--- Classification Results ---" << RESET << "\n";
    for (auto& c : classes) {
        cout << c.className << ": ";
        if (result[c.className].empty()) {
            cout << RED << "-" << RESET << "\n";
        }
        else {
            cout << GREEN;
            for (size_t i = 0; i < result[c.className].size(); i++) {
                cout << result[c.className][i];
                if (i + 1 < result[c.className].size()) cout << ", ";
            }
            cout << RESET << "\n";
        }
    }
    cout << CYAN << "-------------------------------" << RESET << "\n";
    cout << GREEN << "✅ Classification completed successfully!" << RESET << "\n";

    // Ask if user wants to save results
    cout << YELLOW << "\nDo you want to save results to a file? (y/n): " << RESET;
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        string outFile;
        cout << YELLOW << "Enter output file name: " << RESET;
        cin >> outFile;
        ofstream fout(outFile);
        if (!fout) {
            cerr << RED << "[ERROR] Cannot create file: " << outFile << RESET << "\n";
        }
        else {
            for (auto& c : classes) {
                fout << c.className << ": ";
                if (result[c.className].empty()) {
                    fout << "-\n";
                }
                else {
                    for (size_t i = 0; i < result[c.className].size(); i++) {
                        fout << result[c.className][i];
                        if (i + 1 < result[c.className].size()) fout << ", ";
                    }
                    fout << "\n";
                }
            }
            cout << GREEN << "[OK] Results saved to " << outFile << RESET << "\n";
        }
    }

    system("pause");
    return 0;
}
