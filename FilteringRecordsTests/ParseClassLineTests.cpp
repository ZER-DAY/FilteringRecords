#include "pch.h"
#include <CppUnitTest.h>
#include "../Rule.h"
#include "../Parser.h"
#include "../Error.h"
#include <set>

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
 * Also checks that parsing errors are properly collected in std::set<Error>.
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
            std::set<Error> errors;
            bool ok = parse_class_line("With coating: has property \"coating\"", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("With coating"s, rule.className);
            Assert::AreEqual("coating"s, rule.rules[0].propertyName);
            Assert::AreEqual((int)RuleType::HAS_PROPERTY, (int)rule.rules[0].type);
        }

        // 2. PROPERTY_SIZE
        TEST_METHOD(PropertySize_ShouldParse)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Voluminous: property \"size\" has 3 values", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("Voluminous"s, rule.className);
            Assert::AreEqual("size"s, rule.rules[0].propertyName);
            Assert::AreEqual(3, rule.rules[0].expectedSize);
        }

        // 3. CONTAINS_VALUE
        TEST_METHOD(ContainsValue_ShouldParse)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Blue: property \"color\" contains value 1", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("Blue"s, rule.className);
            Assert::AreEqual("color"s, rule.rules[0].propertyName);
            Assert::AreEqual(1, rule.rules[0].expectedValue);
        }

        // 4. EQUALS_EXACTLY
        TEST_METHOD(EqualsExactly_ShouldParse)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Matte: property \"coating\" = [44, 21]", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("Matte"s, rule.className);
            Assert::AreEqual("coating"s, rule.rules[0].propertyName);
            Assert::AreEqual((int)RuleType::EQUALS_EXACTLY, (int)rule.rules[0].type);
            Assert::AreEqual(2, (int)rule.rules[0].expectedExactValues.size());
        }

        // 5. Invalid syntax
        TEST_METHOD(Invalid_Syntax_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Class: if \"size\" = number", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 6. Spaces inside
        TEST_METHOD(SpacesInside_ShouldWork)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Green : property \"color\" contains value 2", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("Green"s, rule.className);
            Assert::AreEqual("color"s, rule.rules[0].propertyName);
            Assert::AreEqual(2, rule.rules[0].expectedValue);
        }

        // 7. Empty class name
        TEST_METHOD(Empty_ClassName_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line(": has property \"x\"", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 8. Missing brackets
        TEST_METHOD(MissingBrackets_EqualsExactly_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Matte: property \"coating\" = 44, 21", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 9. Invalid value (non-numeric)
        TEST_METHOD(InvalidValueText_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Blue: property \"color\" contains value X", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 10. Missing quotes
        TEST_METHOD(MissingQuotes_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Red: property color contains value 1", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 11. Unknown rule type
        TEST_METHOD(UnknownRuleType_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Orange: property \"a\" something else", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 12. Extra spaces
        TEST_METHOD(ExtraSpaces_ShouldWork)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Green: property \"color\" contains value 2", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("Green"s, rule.className);
        }

        // 13. Missing brackets again
        TEST_METHOD(MissingBrackets2_EqualsExactly_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Matte: property \"coating\" = 44, 21", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 14. Text instead of number again
        TEST_METHOD(InvalidValueText2_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Blue: property \"color\" contains value XX", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 15. Missing quotes again
        TEST_METHOD(MissingQuotes2_ShouldFail)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line("Red: property color contains value 1", rule, errors);

            Assert::IsFalse(ok);
            Assert::IsTrue(errors.size() > 0);
        }

        // 16. Spaces around
        TEST_METHOD(SpacesAround_ShouldWork)
        {
            ClassRule rule;
            std::set<Error> errors;
            bool ok = parse_class_line(" Green: property \"color\" contains value 2 ", rule, errors);

            Assert::IsTrue(ok);
            Assert::AreEqual(size_t(0), errors.size());
            Assert::AreEqual("Green"s, rule.className);
        }
    };
}
