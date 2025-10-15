#include "pch.h"
#include <CppUnitTest.h>
#include "../Rule.h"
#include "../Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

/*
 * Test Suite: ParseClassLineTests
 * -------------------------------
 * Tests the correctness of class rule parsing logic.
 * Verifies if parse_class_line correctly interprets supported rule patterns:
 *  - has property "name"
 *  - property "name" has N values
 *  - property "name" contains value X
 *  - property "name" = [a, b, c]
 *  - and various invalid/edge cases
 */

namespace ParseClassLineTests
{
    TEST_CLASS(ParseClassLineTests)
    {
    public:

        // 1. HAS_PROPERTY
        TEST_METHOD(HasProperty_ShouldParse)
        {
            ClassRule rule;
            bool ok = parse_class_line("With coating: has property \"coating\"", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("With coating"s, rule.className);
            Assert::AreEqual("coating"s, rule.rules[0].propertyName);
            Assert::AreEqual((int)RuleType::HAS_PROPERTY, (int)rule.rules[0].type);
        }

        // 2. PROPERTY_SIZE
        TEST_METHOD(PropertySize_ShouldParse)
        {
            ClassRule rule;
            bool ok = parse_class_line("Voluminous: property \"size\" has 3 values", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Voluminous"s, rule.className);
            Assert::AreEqual("size"s, rule.rules[0].propertyName);
            Assert::AreEqual(3, rule.rules[0].expectedSize);
        }

        // 3. CONTAINS_VALUE
        TEST_METHOD(ContainsValue_ShouldParse)
        {
            ClassRule rule;
            bool ok = parse_class_line("Blue: property \"color\" contains value 1", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Blue"s, rule.className);
            Assert::AreEqual("color"s, rule.rules[0].propertyName);
            Assert::AreEqual(1, rule.rules[0].expectedValue);
        }

        // 4. EQUALS_EXACTLY
        TEST_METHOD(EqualsExactly_ShouldParse)
        {
            ClassRule rule;
            bool ok = parse_class_line("Matte: property \"coating\" = [44, 21]", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Matte"s, rule.className);
            Assert::AreEqual("coating"s, rule.rules[0].propertyName);
            Assert::AreEqual((int)RuleType::EQUALS_EXACTLY, (int)rule.rules[0].type);
            Assert::AreEqual(2, (int)rule.rules[0].expectedExactValues.size());
        }

        // 5. Ошибочный формат
        TEST_METHOD(Invalid_Syntax_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Class: if \"size\" = number", rule);
            Assert::IsFalse(ok);
        }

        // 6. Пробелы внутри
        TEST_METHOD(SpacesInside_ShouldWork)
        {
            ClassRule rule;
            bool ok = parse_class_line("Green : property \"color\" contains value 2", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Green"s, rule.className);
            Assert::AreEqual("color"s, rule.rules[0].propertyName);
            Assert::AreEqual(2, rule.rules[0].expectedValue);
        }

        // 7. Пустое имя класса
        TEST_METHOD(Empty_ClassName_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line(": has property \"x\"", rule);
            Assert::IsFalse(ok);
        }

        // 8. Отсутствуют скобки в EQUALS_EXACTLY
        TEST_METHOD(MissingBrackets_EqualsExactly_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Matte: property \"coating\" = 44, 21", rule);
            Assert::IsFalse(ok);
        }

        // 9. Текст вместо числа в CONTAINS_VALUE
        TEST_METHOD(InvalidValueText_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Blue: property \"color\" contains value X", rule);
            Assert::IsFalse(ok);
        }

        // 10. Пропущены кавычки
        TEST_METHOD(MissingQuotes_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Red: property color contains value 1", rule);
            Assert::IsFalse(ok);
        }

        // 11. Несуществующий тип правила
        TEST_METHOD(UnknownRuleType_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Orange: property \"a\" something else", rule);
            Assert::IsFalse(ok);
        }

        // 12. Лишние пробелы
        TEST_METHOD(ExtraSpaces_ShouldWork)
        {
            ClassRule rule;
            bool ok = parse_class_line("Green: property \"color\" contains value 2", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Green"s, rule.className);
        }

        // 13. Пропущены скобки (повтор 8)
        TEST_METHOD(MissingBrackets2_EqualsExactly_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Matte: property \"coating\" = 44, 21", rule);
            Assert::IsFalse(ok);
        }

        // 14. Текст вместо числа (повтор 9)
        TEST_METHOD(InvalidValueText2_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Blue: property \"color\" contains value XX", rule);
            Assert::IsFalse(ok);
        }

        // 15. Пропущены кавычки (многострочный)
        TEST_METHOD(MissingQuotes2_ShouldFail)
        {
            ClassRule rule;
            bool ok = parse_class_line("Red: property color contains value 1", rule);
            Assert::IsFalse(ok);
        }

        // 16. Лишние пробелы в начале и конце
        TEST_METHOD(SpacesAround_ShouldWork)
        {
            ClassRule rule;
            bool ok = parse_class_line(" Green: property \"color\" contains value 2 ", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Green"s, rule.className);
        }
    };
}
