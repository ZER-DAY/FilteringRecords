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
 */

namespace ParseClassLineTests
{
    TEST_CLASS(ParseClassLineTests)
    {
    public:

        TEST_METHOD(Correct_HasProperty)
        {
            ClassRule rule;
            bool ok = parse_class_line("With coating: has property \"coating\"", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("With coating"s, rule.className);
            Assert::AreEqual("coating"s, rule.rules[0].propertyName);
            Assert::AreEqual((int)RuleType::HAS_PROPERTY, (int)rule.rules[0].type);
        }

        TEST_METHOD(Correct_PropertySize)
        {
            ClassRule rule;
            bool ok = parse_class_line("Voluminous: property \"size\" has 3 values", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Voluminous"s, rule.className);
            Assert::AreEqual("size"s, rule.rules[0].propertyName);
            Assert::AreEqual(3, rule.rules[0].expectedSize);
        }

        TEST_METHOD(Correct_ContainsValue)
        {
            ClassRule rule;
            bool ok = parse_class_line("Blue: property \"color\" contains value 1", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Blue"s, rule.className);
            Assert::AreEqual("color"s, rule.rules[0].propertyName);
            Assert::AreEqual(1, rule.rules[0].expectedValue);
        }

        TEST_METHOD(Correct_EqualsExactly)
        {
            ClassRule rule;
            bool ok = parse_class_line("Matte: property \"coating\" = [44, 21]", rule);
            Assert::IsTrue(ok);
            Assert::AreEqual("Matte"s, rule.className);
            Assert::AreEqual("coating"s, rule.rules[0].propertyName);
            Assert::AreEqual((int)RuleType::EQUALS_EXACTLY, (int)rule.rules[0].type);
            Assert::AreEqual(2, (int)rule.rules[0].expectedExactValues.size());
        }

        TEST_METHOD(Invalid_Syntax)
        {
            ClassRule rule;
            bool ok = parse_class_line("Class: if \"size\" = number", rule);
            Assert::IsFalse(ok);
        }

        TEST_METHOD(Empty_ClassName)
        {
            ClassRule rule;
            bool ok = parse_class_line(": has property \"x\"", rule);
            Assert::IsFalse(ok);
        }
    };
}
