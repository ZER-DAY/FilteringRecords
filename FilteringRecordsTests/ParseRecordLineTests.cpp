#include "pch.h"
#include "CppUnitTest.h"

#include "../Record.h"   
#include "../Parser.h"   
#include "../Error.h" 

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ParseRecordLineTests
{
    TEST_CLASS(ParseRecordLineTests)
    {
    public:

        // 1. Корректная запись
        TEST_METHOD(CorrectRecord_ShouldParse)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Wardrobe: color = [1, 2], size = [10, 20]", r, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(std::string("Wardrobe"), r.name);
            Assert::AreEqual(size_t(2), r.properties.size());
            Assert::IsNotNull(r.getProperty("color"));
            Assert::IsNotNull(r.getProperty("size"));
            Assert::AreEqual(2, (int)r.getProperty("color")->values.size());
        }

        // 2. Некорректное значение
        TEST_METHOD(IncorrectValue_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Table: color = [1, a, 3]", r, errors);
            Assert::IsFalse(ok);
        }

        // 3. Дублирующееся свойство
        TEST_METHOD(DuplicateProperty_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Chair: size=[10], size=[20]", r, errors);
            Assert::IsFalse(ok);
        }

        // 4. Нет свойств
        TEST_METHOD(NoProperties_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Lamp:", r, errors);
            Assert::IsFalse(ok);
        }

        // 5. Имя пустое
        TEST_METHOD(EmptyName_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line(": color=[1]", r, errors);
            Assert::IsFalse(ok);
        }

        // 6. Незакрытая скобка
        TEST_METHOD(UnclosedBracket_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Chair: size=[10,20", r, errors);
            Assert::IsFalse(ok);
        }

        // 7. Двойные скобки
        TEST_METHOD(DoubleBrackets_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Table: color=[[1,2]]", r, errors);
            Assert::IsFalse(ok);
        }

        // 8. Только одно свойство
        TEST_METHOD(SingleProperty_ShouldWork)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Lamp: power=[100]", r, errors);
            Assert::IsTrue(ok);
            Assert::AreEqual(std::string("Lamp"), r.name);
            Assert::IsNotNull(r.getProperty("power"));
        }

        // 9. Пробелы вокруг имени
        TEST_METHOD(ExtraSpaces_ShouldWork)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("   Sofa   : color = [ 5 ]", r, errors);
            Assert::IsTrue(ok);
            Assert::AreEqual(std::string("Sofa"), r.name);
            Assert::IsNotNull(r.getProperty("color"));
            Assert::AreEqual(1, (int)r.getProperty("color")->values.size());
        }

        // 10. Пробелы вокруг значений
        TEST_METHOD(SpacesAroundValues_ShouldWork)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Box: length = [  50  ]", r, errors);
            Assert::IsTrue(ok);
            Assert::AreEqual(1, (int)r.getProperty("length")->values.size());
            Assert::AreEqual(50, r.getProperty("length")->values[0]);
        }

        // 11. Очень много свойств (stress test)
        TEST_METHOD(ManyProperties_StressTest)
        {
            std::string line = "Item:";
            for (int i = 0; i < 1000; i++) {
                if (i > 0) line += ",";
                line += " p" + std::to_string(i) + "=[1]";
            }
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line(line, r, errors);
            Assert::IsTrue(ok);
            Assert::AreEqual((size_t)1000, r.properties.size());
        }

        // 12. Повтор свойства с разным регистром
        TEST_METHOD(DifferentCaseProperty_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Item: Size=[10], size=[20]", r, errors);
            Assert::IsFalse(ok);
        }

        // 13. Пустая строка
        TEST_METHOD(EmptyLine_ShouldFail)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("", r, errors);
            Assert::IsFalse(ok);
        }

        // 14. Скобки без значений
        TEST_METHOD(EmptyBrackets_ShouldWork)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Shelf: height = []", r, errors);
            Assert::IsTrue(ok);
            Assert::IsNotNull(r.getProperty("height"));
            Assert::AreEqual((int)0, (int)r.getProperty("height")->values.size());
        }

        // 15. Отрицательные значения
        TEST_METHOD(NegativeValues_ShouldWork)
        {
            Record r;
            std::set<Error> errors;
            bool ok = parse_record_line("Desk: width=[-5, -10]", r, errors);
            Assert::IsTrue(ok);
            Assert::IsNotNull(r.getProperty("width"));
            Assert::AreEqual(2, (int)r.getProperty("width")->values.size());
            Assert::AreEqual(-5, r.getProperty("width")->values[0]);
        }
    };
}
