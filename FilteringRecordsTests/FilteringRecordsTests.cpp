#include "pch.h"
#include <CppUnitTest.h>
#include <vector>
#include <string>
#include "../Record.h"
#include "../Rule.h"
#include "../Validation.h"
#include "../Classifier.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

/*
 * Test Suite: FilteringRecordsTests
 * ---------------------------------
 * Comprehensive unit tests for the RecordClassifier project.
 *
 * Covered functions:
 *   - validate_records
 *   - validate_classes
 *   - classify
 */

namespace FilteringRecordsTests
{
    TEST_CLASS(FilteringRecordsTests)
    {
    public:

        //---------------------------------------------
        // TESTS FOR validate_records()
        //---------------------------------------------
        TEST_METHOD(ValidateRecords_Correct)
        {
            Record r1{ "Wardrobe", {{"color", {"color", {1, 2}}}, {"size", {"size", {10, 40, 60}}}} };
            Record r2{ "Table", {{"color", {"color", {1, 4}}}, {"size", {"size", {20, 40}}}, {"coating", {"coating", {44}}}} };

            vector<Record> records = { r1, r2 };
            auto result = validate_records(records);
            Assert::IsTrue(result.isCorrect);
            Assert::AreEqual(""s, result.reason);
        }

        TEST_METHOD(ValidateRecords_Empty)
        {
            vector<Record> records;
            auto result = validate_records(records);
            Assert::IsFalse(result.isCorrect);
            Assert::AreEqual("No records found in input file"s, result.reason);
        }
        TEST_METHOD(ValidateRecords_NoDuplicateProperty)
        {
            Record r{ "Chair", {{"size", {"size", {10}}}, {"width", {"width", {20}}}} };
            vector<Record> records = { r };
            auto result = validate_records(records);
            Assert::IsTrue(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_EmptyProperties)
        {
            Record r{ "Lamp", {} };
            vector<Record> records = { r };
            auto result = validate_records(records);
            Assert::IsFalse(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_NegativeValues)
        {
            Record r{ "Shelf", {{"depth", {"depth", {-5}}}} };
            vector<Record> records = { r };
            auto result = validate_records(records);
            Assert::IsTrue(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_MultipleValidRecords)
        {
            Record r1{ "Wardrobe", {{"color", {"color", {1}}}} };
            Record r2{ "Table", {{"size", {"size", {10}}}} };
            Record r3{ "Sofa", {{"height", {"height", {20}}}} };
            vector<Record> records = { r1, r2, r3 };
            auto result = validate_records(records);
            Assert::IsTrue(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_EmptyName)
        {
            Record r{ "", {{"color", {"color", {1}}}} };
            vector<Record> records = { r };
            auto result = validate_records(records);
            Assert::IsFalse(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_DuplicateRecordNames)
        {
            Record r1{ "Wardrobe", {{"color", {"color", {1}}}} };
            Record r2{ "Wardrobe", {{"size", {"size", {2}}}} };
            vector<Record> records = { r1, r2 };
            auto result = validate_records(records);
            Assert::IsTrue(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_ManyProperties_Stress)
        {
            Record big{ "Big", {} };
            for (int i = 0; i < 1000; i++)
                big.properties["p" + to_string(i)] = { "p" + to_string(i), {i} };
            vector<Record> records = { big };
            auto result = validate_records(records);
            Assert::IsTrue(result.isCorrect);
        }

        TEST_METHOD(ValidateRecords_DifferentCaseProperty)
        {
            Record r{ "Item", {{"Size", {"Size", {10}}}, {"size", {"size", {20}}}} };
            vector<Record> records = { r };
            auto result = validate_records(records);
            Assert::IsFalse(result.isCorrect);
        }

        //---------------------------------------------
        // TESTS FOR validate_classes()
        //---------------------------------------------
        TEST_METHOD(ValidateClasses_Correct)
        {
            Rule r1{ RuleType::HAS_PROPERTY, "coating" };
            Rule r2{ RuleType::PROPERTY_SIZE, "size", 3 };
            ClassRule c1{ "With coating", { r1 } };
            ClassRule c2{ "Voluminous", { r2 } };

            vector<ClassRule> classes = { c1, c2 };
            auto result = validate_classes(classes);
            Assert::IsTrue(result.isCorrect);
        }

        TEST_METHOD(ValidateClasses_Empty)
        {
            vector<ClassRule> classes;
            auto result = validate_classes(classes);
            Assert::IsFalse(result.isCorrect);
            Assert::AreEqual("No classes or rules found in input file"s, result.reason);
        }

        //---------------------------------------------
        // TESTS FOR classify()
        //---------------------------------------------
        TEST_METHOD(Classify_Simple)
        {
            // Arrange
            Record wardrobe{ "Wardrobe", {{"color", {"color", {1, 2}}}, {"size", {"size", {10, 40, 60}}}} };
            Record table{ "Table", {{"color", {"color", {1, 4}}}, {"size", {"size", {20, 40}}}, {"coating", {"coating", {44}}}} };
            vector<Record> records = { wardrobe, table };

            Rule r1{ RuleType::HAS_PROPERTY, "coating" };
            Rule r2{ RuleType::PROPERTY_SIZE, "size", 3 };
            Rule r3{ RuleType::CONTAINS_VALUE, "color", 1 };
            Rule r4{ RuleType::EQUALS_EXACTLY, "coating", 0, 0, {44, 21} };

            ClassRule c1{ "With coating", { r1 } };
            ClassRule c2{ "Voluminous", { r2 } };
            ClassRule c3{ "Blue", { r3 } };
            ClassRule c4{ "Matte", { r4 } };
            vector<ClassRule> classes = { c1, c2, c3, c4 };

            auto result = classify(records, classes);

            Assert::AreEqual(string("Table"), result["With coating"][0]);
            Assert::AreEqual(string("Wardrobe"), result["Voluminous"][0]);
            Assert::AreEqual(0u, (unsigned)result["Blue"].size());
            Assert::IsTrue(result["Matte"].empty());
        }

        TEST_METHOD(Classify_EmptyRules)
        {
            Record r{ "Box", {{"weight", {"weight", {10}}}} };
            vector<Record> records = { r };
            vector<ClassRule> classes;
            auto result = classify(records, classes);
            Assert::IsTrue(result.empty());
        }

        TEST_METHOD(Classify_MultipleMatches)
        {
            Record car{ "Car", {{"color", {"color", {2}}}, {"doors", {"doors", {4}}}, {"engine", {"engine", {2000}}}} };
            Record bus{ "Bus", {{"color", {"color", {2, 3}}}, {"doors", {"doors", {2}}}, {"seats", {"seats", {40}}}} };
            vector<Record> records = { car, bus };

            Rule r1{ RuleType::HAS_PROPERTY, "doors" };
            Rule r2{ RuleType::CONTAINS_VALUE, "color", 2 };
            Rule r3{ RuleType::EQUALS_EXACTLY, "seats", 0, 0, {40} };
            ClassRule c1{ "Has doors", { r1 } };
            ClassRule c2{ "Red vehicle", { r2 } };
            ClassRule c3{ "Big bus", { r3 } };
            vector<ClassRule> classes = { c1, c2, c3 };

            auto result = classify(records, classes);

            Assert::AreEqual(2u, (unsigned)result["Has doors"].size());
            Assert::AreEqual(0u, (unsigned)result["Red vehicle"].size());
            Assert::AreEqual(string("Bus"), result["Big bus"][0]);
        }
    };
}
