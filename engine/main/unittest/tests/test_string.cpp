#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class StringTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Register());
		W_TEST_SUCCESS(GlobalAllocators::Initialize());
	}

	ON_AFTER_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Shutdown());
		W_TEST_SUCCESS(GlobalAllocators::Unregister());
	}
};

TEST_SUITE(StringTest)

TEST_CASE(StringTest, DefaultConstructor)
{
	String str;
	TEST_EQUAL(str.Length(), 0);

	String str2("Test");
	TEST_EQUAL(str2.Length(), 4);
	TEST_EQUAL(strcmp(str2.CStr(), "Test"), 0);
}

TEST_CASE(StringTest, LongString)
{
	const char* longStr = "This is a long string that exceeds the short string optimization size.";
	String		str(longStr);
	TEST_EQUAL(str.Length(), strlen(longStr));
	TEST_EQUAL(strcmp(str.CStr(), longStr), 0);
}

TEST_CASE(StringTest, MoveConstructor)
{
	String str1("Hello");
	String str2((String&&)str1);
	String str3 = (String&&)str2;

	TEST_EQUAL(str3.Length(), 5);
	TEST_EQUAL(strcmp(str3.CStr(), "Hello"), 0);
}

TEST_CASE(StringTest, AssignmentOperator)
{
	String str1("Hello");
	String str2;
	str2 = (String&&)str1;

	TEST_EQUAL(str2.Length(), 5);
	TEST_EQUAL(strcmp(str2.CStr(), "Hello"), 0);

	String str3;
	str3 = "World";

	TEST_EQUAL(str3.Length(), 5);
	TEST_EQUAL(strcmp(str3.CStr(), "World"), 0);
}

TEST_CASE(StringTest, ConcatenationOperator)
{
	String str1("Hello");
	String str2(" World");
	String str3 = str1 + str2;

	TEST_EQUAL(str3.Length(), 11);
	TEST_EQUAL(strcmp(str3.CStr(), "Hello World"), 0);
}

TEST_CASE(StringTest, Clear)
{
	String str("Hello");
	TEST_EQUAL(str.Length(), 5);
	TEST_SUCCESS(str.Clear());
	TEST_EQUAL(str.Length(), 0);
}

TEST_CASE(StringTest, EndsWith)
{
	String str("Hello World");
	TEST_ASSERT(str.EndsWith("World"));
	TEST_ASSERT(!str.EndsWith("Hello"));
	TEST_ASSERT(!str.EndsWith("Hello World!"));
}

TEST_CASE(StringTest, Slice)
{
	String	   str("Hello World");
	StringView slice = str.Slice(6, 5);

	TEST_EQUAL(slice.Length(), 5);
	TEST_EQUAL(strcmp(slice.Data(), "World"), 0);

	StringView slice2 = str.Slice(6, 10);
	TEST_EQUAL(slice2.Length(), 5); // Should be clamped to the end of the string
	TEST_EQUAL(strcmp(slice2.Data(), "World"), 0);

	StringView slice3 = str.Slice(13, 2);
	TEST_EQUAL(slice3.Length(), 0);		// Out of bounds, should return empty slice
	TEST_EQUAL(slice3.Data(), nullptr); // Data should be nullptr for empty slice
}