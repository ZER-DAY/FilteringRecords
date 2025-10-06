#include "pch.h"
#include <CppUnitTest.h>
#include "../Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

/*
 * Test Suite: TrimTests
 * ---------------------
 * Tests the utility function trim() that removes
 * leading and trailing spaces and tabs from strings.
 */

namespace TrimTests
{
    TEST_CLASS(TrimTests)
    {
    public:

        TEST_METHOD(RemovesLeadingSpaces)
        {
            string input = "   hello";
            string output = trim(input);
            Assert::AreEqual("hello"s, output);
        }

        TEST_METHOD(RemovesTrailingSpaces)
        {
            string input = "world   ";
            string output = trim(input);
            Assert::AreEqual("world"s, output);
        }

        TEST_METHOD(RemovesBothSides)
        {
            string input = "   test case   ";
            string output = trim(input);
            Assert::AreEqual("test case"s, output);
        }

        TEST_METHOD(RemovesTabs)
        {
            string input = "\t\tTrim me\t\t";
            string output = trim(input);
            Assert::AreEqual("Trim me"s, output);
        }

        TEST_METHOD(EmptyString)
        {
            string input = "";
            string output = trim(input);
            Assert::AreEqual(""s, output);
        }

        TEST_METHOD(OnlySpaces)
        {
            string input = "     ";
            string output = trim(input);
            Assert::AreEqual(""s, output);
        }
    };
}
