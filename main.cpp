#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Record.h"
#include "Rule.h"
#include "Parser.h"
#include "Classifier.h"
#include "Validation.h"
#include "Error.h"
#include <set>
using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

//void print_help() {
//    cout << CYAN << "=====================================\n";
//    cout << "   RecordClassifier - Help Guide\n";
//    cout << "=====================================" << RESET << "\n\n";
//
//    cout << YELLOW << "Usage:\n" << RESET;
//    cout << "  FilteringRecords.exe <items_file> <rules_file> [output_file]\n\n";
//
//    cout << YELLOW << "Examples:\n" << RESET;
//    cout << "  FilteringRecords.exe items.txt rules.txt\n";
//    cout << "  FilteringRecords.exe items.txt rules.txt output.txt\n";
//    cout << "  FilteringRecords.exe -h\n\n";
//
//    cout << YELLOW << "Input Format:\n" << RESET;
//    cout << "  Records (items):\n";
//    cout << "    <RecordName>: <property> = [values], ...\n";
//    cout << "  Rules:\n";
//    cout << "    <ClassName>: has property \"x\"\n";
//    cout << "    <ClassName>: property \"x\" has N values\n";
//    cout << "    <ClassName>: property \"x\" contains value Y\n";
//    cout << "    <ClassName>: property \"x\" = [a, b, c]\n\n";
//}

int main(int argc, char* argv[]) {
    cout << CYAN << "=====================================\n";
    cout << "     RecordClassifier - v2.2\n";
    cout << "   Items + Rules --> Output\n";
    cout << "=====================================" << RESET << "\n";

    //// --- Handle arguments ---
    //if (argc == 2 && (string(argv[1]) == "-h" || string(argv[1]) == "--help")) {
    //    print_help();
    //    return 0;
    //}

    if (argc < 3) {
        cerr << RED << "[ERROR] Not enough arguments.\n"
            << "Usage: FilteringRecords.exe <items_file> <rules_file> [output_file]\n"
            << "Use -h for help." << RESET << endl;
        return 1;
    }

    string itemsFile = argv[1];
    string rulesFile = argv[2];
    string outputFile = (argc >= 4) ? argv[3] : "output.txt";

    // --- Open input files ---
    ifstream items(itemsFile);
    if (!items) {
        cerr << RED << "[ERROR] Cannot open file: " << itemsFile << RESET << endl;
        return 1;
    }

    ifstream rules(rulesFile);
    if (!rules) {
        cerr << RED << "[ERROR] Cannot open file: " << rulesFile << RESET << endl;
        return 1;
    }

    cout << YELLOW << "[INFO] Reading items from: " << itemsFile << RESET << endl;
    cout << YELLOW << "[INFO] Reading rules from: " << rulesFile << RESET << endl;

    // --- Parse items ---
    vector<Record> records;
    string line;
    while (getline(items, line)) {
        string clean = trim(line);
        if (clean.empty()) continue;

        Record r;
        if (!parse_record_line(clean, r)) {
            cerr << RED << "[ERROR] Invalid record format: " << clean << RESET << endl;
            return 1;
        }
        records.push_back(r);
    }

    // --- Error system ---
    std::set<Error> errors;
    vector<ClassRule> classes;

    // --- Parse rules ---
    while (getline(rules, line)) {
        string clean = trim(line);
        if (clean.empty()) continue;

        ClassRule cr;
        if (!parse_class_line(clean, cr, errors)) {
            cerr << YELLOW << "[WARN] Invalid rule format: " << clean << RESET << endl;
            // لا نوقف التنفيذ — نتابع قراءة باقي القواعد
            continue;
        }

        classes.push_back(cr);
    }

    // --- بعد الانتهاء من التحليل، عرض الأخطاء ---
    if (!errors.empty()) {
        cout << RED << "\n[WARN] Some rules contain errors:\n" << RESET;
        cout << CYAN << "------------------------------------------------------------\n";
        cout << "Code                  | Message                                 | Source\n";
        cout << "------------------------------------------------------------" << RESET << endl;

        for (const auto& e : errors) {
            string codeStr = e.codeToString(e.code);
            cout << codeStr
                << string(22 - min<size_t>(codeStr.length(), 22), ' ')
                << " | " << e.message
                << string(40 - min<size_t>(e.message.length(), 40), ' ')
                << " | " << e.source << endl;
        }

        cout << CYAN << "------------------------------------------------------------" << RESET << endl;
        cout << YELLOW << errors.size() << " error(s) detected. "
            << "Output will include only valid rules.\n" << RESET;
    }

    // --- Validation ---
    DataCheckResult recCheck = validate_records(records);
    if (!recCheck.isCorrect) {
        cerr << RED << "[ERROR] " << recCheck.reason << RESET << endl;
        return 1;
    }

    DataCheckResult clsCheck = validate_classes(classes);
    if (!clsCheck.isCorrect) {
        cerr << RED << "[ERROR] " << clsCheck.reason << RESET << endl;
        return 1;
    }

    // --- Classification ---
    cout << CYAN << "[INFO] Running classification..." << RESET << endl;
    auto result = classify(records, classes);

    // --- Output ---
    ofstream fout(outputFile);
    if (!fout) {
        cerr << RED << "[ERROR] Cannot create file: " << outputFile << RESET << endl;
        return 1;
    }

    fout << "--------------------------------------------\n";
    fout << "Class          | Matching Records\n";
    fout << "--------------------------------------------\n";
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

    fout << "--------------------------------------------\n";
    fout << "[OK] Classification completed.\n";

    cout << GREEN << "[OK] Results written to: " << outputFile << RESET << endl;
    return 0;
}
