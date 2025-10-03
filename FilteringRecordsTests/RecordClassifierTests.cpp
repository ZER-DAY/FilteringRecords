#include "pch.h"
#include <CppUnitTest.h>
#include <vector>
#include <string>
#include "../FilteringRecords/Record.h"
#include "../FilteringRecords/Rule.h"
#include "../FilteringRecords/Parser.h"
#include "../FilteringRecords/Classifier.h"
#include "../FilteringRecords/Validation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::string_literals;

namespace Tests
{
    TEST_CLASS(RecordClassifierTests)
    {
    public:

        // --- FUNCTIONAL CLASSIFICATION TESTS ---

        TEST_METHOD(Test_HasProperty)
        {
            Record r1; r1.name = "Table";
            r1.properties["legs"] = Property{ "legs",{4} };

            Record r2; r2.name = "Lamp";
            r2.properties["light"] = Property{ "light",{1} };

            std::vector<Record> records = { r1, r2 };

            Rule rule{ RuleType::HAS_PROPERTY, "legs" };
            ClassRule cr{ "HasLegs",{rule} };

            auto result = classify(records, { cr });
            Assert::AreEqual("Table"s, result["HasLegs"][0]);
        }

        TEST_METHOD(Test_PropertySize)
        {
            Record r; r.name = "Wardrobe";
            r.properties["size"] = Property{ "size",{10,20,30} };

            std::vector<Record> records = { r };

            Rule rule{ RuleType::PROPERTY_SIZE,"size",3 };
            ClassRule cr{ "Voluminous",{rule} };

            auto result = classify(records, { cr });
            Assert::AreEqual("Wardrobe"s, result["Voluminous"][0]);
        }

        TEST_METHOD(Test_ContainsValue)
        {
            Record r; r.name = "Car";
            r.properties["color"] = Property{ "color",{1,5} };

            std::vector<Record> records = { r };

            Rule rule{ RuleType::CONTAINS_VALUE,"color",0,1 };
            ClassRule cr{ "Blue",{rule} };

            auto result = classify(records, { cr });
            Assert::AreEqual("Car"s, result["Blue"][0]);
        }

        TEST_METHOD(Test_EqualsExactly)
        {
            Record r; r.name = "Special";
            r.properties["code"] = Property{ "code",{44,21} };

            std::vector<Record> records = { r };

            Rule rule{ RuleType::EQUALS_EXACTLY,"code",0,0,{44,21} };
            ClassRule cr{ "Matte",{rule} };

            auto result = classify(records, { cr });
            Assert::AreEqual("Special"s, result["Matte"][0]);
        }

        // --- VALIDATION TESTS ---

        TEST_METHOD(Test_DuplicatePropertyName)
        {
            Record r; r.name = "Bad";
            r.properties["color"] = Property{ "color",{1} };
            // simulate duplicate by overriding
            r.properties["color"] = Property{ "color",{2} };

            auto check = validate_records({ r });
            Assert::AreEqual(false, check.isCorrect);
            Assert::AreEqual("Duplicate property name detected"s, check.reason);
        }

        TEST_METHOD(Test_EmptyProperties)
        {
            Record r; r.name = "Empty";

            auto check = validate_records({ r });
            Assert::AreEqual(false, check.isCorrect);
            Assert::AreEqual("No properties defined for record"s, check.reason);
        }

        TEST_METHOD(Test_NoRecords)
        {
            std::vector<Record> records = {};
            auto check = validate_records(records);
            Assert::AreEqual(false, check.isCorrect);
            Assert::AreEqual("No records found in input file"s, check.reason);
        }

        TEST_METHOD(Test_NoClasses)
        {
            std::vector<ClassRule> classes = {};
            auto check = validate_classes(classes);
            Assert::AreEqual(false, check.isCorrect);
            Assert::AreEqual("No classes or rules found in input file"s, check.reason);
        }

        TEST_METHOD(Test_TypeMismatch)
        {
            // هنا نفترض إنه القيم لازم تكون أعداد صحيحة
            // نعمل Property فيه values فاضية = خطأ
            Property p{ "size",{} };
            Record r; r.name = "Broken";
            r.properties["size"] = p;

            auto check = validate_records({ r });
            Assert::AreEqual(false, check.isCorrect);
        }

        // --- PARSER TESTS (optional) ---

        TEST_METHOD(Test_ParseRecordLine)
        {
            std::string line = "Car: color = [1, 2], doors = [4]";
            Record r;
            bool ok = parse_record_line(line, r);

            Assert::IsTrue(ok);
            Assert::AreEqual("Car"s, r.name);
            Assert::AreEqual(size_t(2), r.properties["color"].values.size());
            Assert::AreEqual(4, r.properties["doors"].values[0]);
        }

        TEST_METHOD(Test_ParseClassLine)
        {
            std::string line = "Blue: property \"color\" contains value 1";
            ClassRule cr;
            bool ok = parse_class_line(line, cr);

            Assert::IsTrue(ok);
            Assert::AreEqual("Blue"s, cr.className);
            Assert::AreEqual(RuleType::CONTAINS_VALUE, cr.rules[0].type);
        }
    };
}
