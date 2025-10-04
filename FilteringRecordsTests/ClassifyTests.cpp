#include "pch.h"
#include <CppUnitTest.h>
#include "../Record.h"
#include "../Rule.h"
#include "../Validation.h"
#include "../Classifier.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
    TEST_CLASS(RecordClassifierTests)
    {
    public:

        TEST_METHOD(ValidateRecords_ShouldPass)
        {
            Property color{ {1, 2} };
            Property size{ {10, 20} };
            Record wardrobe{ "Wardrobe", {{"color", color}, {"size", size}} };

            std::vector<Record> records{ wardrobe };
            auto res = validate_records(records);

            Assert::IsTrue(res.isCorrect);
        }

        TEST_METHOD(ValidateRecords_ShouldFailWhenEmpty)
        {
            std::vector<Record> records;
            auto res = validate_records(records);

            Assert::IsFalse(res.isCorrect);
        }

        TEST_METHOD(ValidateClasses_ShouldPass)
        {
            Rule r1{ RuleType::HAS_PROPERTY, "coating", 0, 0, {} };
            Rule r2{ RuleType::CONTAINS_VALUE, "color", 0, 1, {} };

            ClassRule c1{ "With coating", {r1} };
            ClassRule c2{ "Blue", {r2} };

            std::vector<ClassRule> classes{ c1, c2 };
            auto res = validate_classes(classes);

            Assert::IsTrue(res.isCorrect);
        }

        TEST_METHOD(ValidateClasses_ShouldFailWhenEmpty)
        {
            std::vector<ClassRule> classes;
            auto res = validate_classes(classes);

            Assert::IsFalse(res.isCorrect);
        }

        TEST_METHOD(Classify_SingleMatch)
        {
            Property color{ {1} };
            Property coating{ {44} };
            Record table{ "Table", {{"color", color}, {"coating", coating}} };

            Rule r1{ RuleType::HAS_PROPERTY, "coating", 0, 0, {} };
            ClassRule class1{ "With coating", {r1} };

            auto result = classify({ table }, { class1 });
            Assert::AreEqual(size_t(1), result["With coating"].size());
            Assert::AreEqual(std::string("Table"), result["With coating"][0]);
        }

        TEST_METHOD(Classify_NoMatch)
        {
            Property color{ {2} };
            Record lamp{ "Lamp", {{"color", color}} };

            Rule r1{ RuleType::EQUALS_EXACTLY, "coating", 0, 0, {44, 21} };
            ClassRule class1{ "Matte", {r1} };

            auto result = classify({ lamp }, { class1 });
            Assert::IsTrue(result["Matte"].empty());
        }
    };
}
