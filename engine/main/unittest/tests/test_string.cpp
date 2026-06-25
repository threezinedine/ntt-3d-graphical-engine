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

TEST_CASE(StringTest, MoveConstructor)
{
	String str1("Hello");
	String str2((String&&)str1);
	String str3 = (String&&)str2;

	TEST_EQUAL(str3.Length(), 5);
	TEST_EQUAL(strcmp(str3.CStr(), "Hello"), 0);
}