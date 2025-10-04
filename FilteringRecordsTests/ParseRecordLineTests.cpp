#include "pch.h"
#include "CppUnitTest.h"

#include "../Record.h"   //"../FilteringRecords/Record.h"
#include "../Parser.h"   // где реализована parse_record_line

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ParseRecordLineTests
{
    TEST_CLASS(ParseRecordLineTests)
    {
    public:

        TEST_METHOD(CorrectRecord_ShouldParse)
        {
            Record r;
            bool ok = parse_record_line("Wardrobe: color = [1, 2], size = [10, 20]", r);

            Assert::IsTrue(ok);
            Assert::AreEqual(std::string("Wardrobe"), r.name);
            Assert::AreEqual(size_t(2), r.properties.size());
            Assert::IsNotNull(r.getProperty("color"));
            Assert::IsNotNull(r.getProperty("size"));
            Assert::AreEqual(2, (int)r.getProperty("color")->values.size());
        }

        TEST_METHOD(IncorrectValue_ShouldFail)
        {
            Record r;
            bool ok = parse_record_line("Table: color = [1, a, 3]", r);

            Assert::IsFalse(ok);
        }

        TEST_METHOD(DuplicateProperty_ShouldFail)
        {
            Record r;
            bool ok = parse_record_line("Chair: size=[10], size=[20]", r);

            Assert::IsFalse(ok);
        }

        TEST_METHOD(NoProperties_ShouldFail)
        {
            Record r;
            bool ok = parse_record_line("Lamp:", r);

            Assert::IsFalse(ok);
        }

        TEST_METHOD(ExtraSpaces_ShouldWork)
        {
            Record r;
            bool ok = parse_record_line("Sofa : color = [ 5 ]", r);

            Assert::IsTrue(ok);
            Assert::AreEqual(std::string("Sofa"), r.name);
            Assert::IsNotNull(r.getProperty("color"));
            Assert::AreEqual(1, (int)r.getProperty("color")->values.size());
        }

        TEST_METHOD(EmptyLine_ShouldFail)
        {
            Record r;
            bool ok = parse_record_line("", r);

            Assert::IsFalse(ok);
        }

        TEST_METHOD(EmptyBrackets_ShouldWork)
        {
            Record r;
            bool ok = parse_record_line("Shelf: height = []", r);

            Assert::IsTrue(ok);
            Assert::IsNotNull(r.getProperty("height"));
            Assert::AreEqual((int)0, (int)r.getProperty("height")->values.size());
        }

        TEST_METHOD(NegativeValues_ShouldWork)
        {
            Record r;
            bool ok = parse_record_line("Desk: width=[-5, -10]", r);

            Assert::IsTrue(ok);
            Assert::IsNotNull(r.getProperty("width"));
            Assert::AreEqual(2, (int)r.getProperty("width")->values.size());
            Assert::AreEqual(-5, r.getProperty("width")->values[0]);
        }
    };
}
