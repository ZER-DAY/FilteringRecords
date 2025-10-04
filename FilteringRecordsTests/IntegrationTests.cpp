#include "pch.h"
#include <CppUnitTest.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <direct.h>   // For _access
#include <io.h>       // For _access
#define fs_exists(path) (_access(path, 0) == 0)
#include "../Parser.h"
#include "../Record.h"
#include "../Rule.h"
#include "../Classifier.h"
#include "../Validation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;


/*
 * Test Suite: IntegrationTests
 * ----------------------------
 * Integration-level tests for the full RecordClassifier workflow.
 *
 * Verifies that:
 *   1. The program can read and parse items and rules files.
 *   2. Validation of records and classes passes.
 *   3. Classification results are as expected.
 *   4. Output file is correctly generated.
 */

namespace IntegrationTests
{
    TEST_CLASS(IntegrationTests)
    {
    public:

        TEST_METHOD(FullPipeline_CorrectData)
        {
            // Arrange – create temporary input files
            ofstream items("test_items.txt");
            items << "Wardrobe: color = [1, 2], size = [10, 40, 60]\n";
            items << "Table: color = [1, 4], size = [20, 40], coating = [44]\n";
            items.close();

            ofstream rules("test_rules.txt");
            rules << "With coating: has property \"coating\"\n";
            rules << "Voluminous: property \"size\" has 3 values\n";
            rules << "Blue: property \"color\" contains value 1\n";
            rules << "Matte: property \"coating\" = [44, 21]\n";
            rules.close();

            const string outputFile = "test_output.txt";

            // Act – parse items
            ifstream fin_items("test_items.txt");
            ifstream fin_rules("test_rules.txt");
            Assert::IsTrue(fin_items.good(), L"Cannot open test_items.txt");
            Assert::IsTrue(fin_rules.good(), L"Cannot open test_rules.txt");

            vector<Record> records;
            vector<ClassRule> classes;
            string line;

            while (getline(fin_items, line))
            {
                Record r;
                Assert::IsTrue(parse_record_line(trim(line), r), L"Record parsing failed");
                records.push_back(r);
            }

            while (getline(fin_rules, line))
            {
                ClassRule c;
                Assert::IsTrue(parse_class_line(trim(line), c), L"Class parsing failed");
                classes.push_back(c);
            }

            // Validation
            auto recCheck = validate_records(records);
            Assert::IsTrue(recCheck.isCorrect, L"Record validation failed");

            auto clsCheck = validate_classes(classes);
            Assert::IsTrue(clsCheck.isCorrect, L"Class validation failed");

            // Classification
            auto result = classify(records, classes);

            // Save to file
            ofstream fout(outputFile);
            Assert::IsTrue(fout.good(), L"Cannot create output file");

            for (auto& c : classes)
            {
                fout << c.className << ": ";
                if (result[c.className].empty()) fout << "-\n";
                else
                {
                    for (size_t i = 0; i < result[c.className].size(); i++)
                    {
                        fout << result[c.className][i];
                        if (i + 1 < result[c.className].size()) fout << ", ";
                    }
                    fout << "\n";
                }
            }
            fout.close();

            // Assert – check expected results
            Assert::AreEqual(string("Table"), result["With coating"][0]);
            Assert::AreEqual(string("Wardrobe"), result["Voluminous"][0]);
            Assert::AreEqual(2u, (unsigned)result["Blue"].size());
            Assert::IsTrue(result["Matte"].empty());

            Assert::IsTrue(fs_exists(outputFile.c_str()), L"Output file was not created");
            remove("test_items.txt");
            remove("test_rules.txt");
            remove("test_output.txt");
        }

        TEST_METHOD(Fail_MissingFiles)
        {
            // Ensure non-existing files cause failure to open
            ifstream missing("no_items.txt");
            Assert::IsFalse(missing.good(), L"Missing file opened unexpectedly");
        }

        TEST_METHOD(Fail_InvalidRecordData)
        {
            // Invalid numeric data
            ofstream items("bad_items.txt");
            items << "Table: color = [1, a, 3]\n"; // 'a' not numeric
            items.close();

            ofstream rules("bad_rules.txt");
            rules << "Blue: property \"color\" contains value 1\n";
            rules.close();

            ifstream fin_items("bad_items.txt");
            ifstream fin_rules("bad_rules.txt");

            vector<Record> records;
            vector<ClassRule> classes;
            string line;

            // Record should fail parsing
            bool parsedOk = true;
            while (getline(fin_items, line))
            {
                Record r;
                parsedOk = parse_record_line(trim(line), r);
                if (!parsedOk) break;
            }
            Assert::IsFalse(parsedOk, L"Invalid record should not parse correctly");

            fin_items.close();
            fin_rules.close();

    
        }
    };
}
