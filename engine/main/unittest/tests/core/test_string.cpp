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

	TEST_EQUAL(slice, "World");

	StringView slice2 = str.Slice(6, 10);
	TEST_EQUAL(slice2, "World");

	StringView slice3 = str.Slice(13, 2);
	TEST_EQUAL(slice3.Length(), 0);		// Out of bounds, should return empty slice
	TEST_EQUAL(slice3.Data(), nullptr); // Data should be nullptr for empty slice
}

TEST_CASE(StringTest, Split)
{
	String str("one,two,three,four");
	String delimiter(",");

	Array<StringView> parts = str.Split(delimiter);

	TEST_EQUAL(parts.GetCount(), 4);
	TEST_EQUAL(parts[0], "one");
	TEST_EQUAL(parts[1], "two");
	TEST_EQUAL(parts[2], "three");
	TEST_EQUAL(parts[3], "four");
}

TEST_CASE(StringTest, SplitWithNoDelimiter)
{
	String str("one,two,three,four");
	String delimiter(";");

	Array<StringView> parts = str.Split(delimiter);

	TEST_EQUAL(parts.GetCount(), 1);
	TEST_EQUAL(parts[0], "one,two,three,four");
}

TEST_CASE(StringTest, SplitWithConsecutiveDelimiters)
{
	String str("one,,two,,,three");
	String delimiter(",");

	Array<StringView> parts = str.Split(delimiter);

	TEST_EQUAL(parts.GetCount(), 6);
	TEST_EQUAL(parts[0], "one");
	TEST_EQUAL(parts[1], "");
	TEST_EQUAL(parts[2], "two");
	TEST_EQUAL(parts[3], "");
	TEST_EQUAL(parts[4], "");
	TEST_EQUAL(parts[5], "three");
}

TEST_CASE(StringTest, SplitWithDelimiterAtEnds)
{
	String str(",one,two,three,");
	String delimiter(",");

	Array<StringView> parts = str.Split(delimiter);

	TEST_EQUAL(parts.GetCount(), 5);
	TEST_EQUAL(parts[0], "");
	TEST_EQUAL(parts[1], "one");
	TEST_EQUAL(parts[2], "two");
	TEST_EQUAL(parts[3], "three");
	TEST_EQUAL(parts[4], "");
}

TEST_CASE(StringTest, Reserve)
{
	String str("Hello");
	TEST_EQUAL(str.Length(), 5);
	TEST_SUCCESS(str.Reserve(20));
	TEST_EQUAL(str.IsShortString(), false);				 // Should now be using heap allocation
	TEST_EQUAL(str.Length(), 5);						 // Length should remain the same
	TEST_EQUAL(strcmp(str.GetHeapBuffer(), "Hello"), 0); // Content should remain the same

	TEST_EQUAL(str.Reserve(3), RESULT_NEW_CAPACITY_TOO_SMALL); // Reserving less than current capacity should do nothing
	TEST_EQUAL(str.Length(), 5);
	TEST_EQUAL(str.IsShortString(), false);				 // Should now be using heap allocation
	TEST_EQUAL(str.Length(), 5);						 // Length should remain the same
	TEST_EQUAL(strcmp(str.GetHeapBuffer(), "Hello"), 0); // Content should remain the same
}

TEST_CASE(StringTest, Join)
{
	Array<String> strings;
	TEST_SUCCESS(strings.Append(String("one")));
	TEST_SUCCESS(strings.Append(String("two")));
	TEST_SUCCESS(strings.Append(String("three")));

	String delimiter(", ");
	String result = String::Join(strings, delimiter);

	TEST_EQUAL(result.Length(), 15);
	TEST_EQUAL(result, "one, two, three");
}

TEST_CASE(StringTest, JoinWithEmptyArray)
{
	Array<String> strings;
	String		  delimiter(", ");
	String		  result = String::Join(strings, delimiter);

	TEST_EQUAL(result.Length(), 0);
	TEST_EQUAL(result, "");
}

TEST_CASE(StringTest, InsertStringIntoStringArray)
{
	Array<String> stringArray;
	TEST_SUCCESS(stringArray.Append(String("Hello")));
	TEST_SUCCESS(stringArray.Append(String("World")));
	TEST_EQUAL(stringArray.GetCount(), 2);
	TEST_EQUAL(stringArray[0], "Hello");
	TEST_EQUAL(stringArray[1], "World");

	stringArray.Insert(String("Inserted"), 1);
	TEST_EQUAL(stringArray.GetCount(), 3);
	TEST_EQUAL(stringArray[0], "Hello");
	TEST_EQUAL(stringArray[1], "Inserted");
	TEST_EQUAL(stringArray[2], "World");
}

TEST_CASE(StringTest, StringViewFind)
{
	String	   str("Hello World");
	StringView view(str);

	TEST_EQUAL(view.Find(StringView("World")), 6);
	TEST_EQUAL(view.Find(StringView("Hello")), 0);
	TEST_EQUAL(view.Find(StringView("NotFound")), NTT_INVALID_INDEX);
}

TEST_CASE(StringTest, StringFind)
{
	String str("Hello World");

	TEST_EQUAL(str.Find(String("World")), 6);
	TEST_EQUAL(str.Find(String("Hello")), 0);
	TEST_EQUAL(str.Find(String("NotFound")), NTT_INVALID_INDEX);
}

TEST_CASE(StringTest, ToStringTest)
{
	String	   str("Hello");
	StringView view = ToString(str);
	TEST_EQUAL(view, "Hello");

	i32		   intValue = 42;
	StringView intView	= ToString(intValue);
	TEST_EQUAL(intView, "42");

	u32		   uintValue = 100;
	StringView uintView	 = ToString(uintValue);
	TEST_EQUAL(uintView, "100");

	bool	   boolValue = true;
	StringView boolView	 = ToString(boolValue);
	TEST_EQUAL(boolView, "true");

	f32		   floatValue = 3.14f;
	StringView floatView  = ToString(floatValue);
	TEST_EQUAL(floatView, "3.140000");
}

TEST_CASE(StringTest, ResetLongMessageNotBeingShortString)
{
	String str("This is a long string that exceeds the short string optimization size.");
	TEST_EQUAL(str.IsShortString(), false);
	TEST_SUCCESS(str.Reset());
	TEST_EQUAL(str.Length(), 0);
	TEST_EQUAL(str.IsShortString(), false); // After reset, it should be a short string
}

TEST_CASE(StringTest, ResetShortMessageBeingShortString)
{
	String str("Short");
	TEST_EQUAL(str.IsShortString(), true);
	TEST_SUCCESS(str.Reset());
	TEST_EQUAL(str.Length(), 0);
	TEST_EQUAL(str.IsShortString(), true); // After reset, it should still be a short string
}

TEST_CASE(StringTest, MoveStringWithDifferentAllocators)
{
	String str1("Hello World!!!!!!!!!!", g_GlobalAllocators.pStack);
	String str2(g_GlobalAllocators.pMalloc);

	u32 strLength	   = str1.Length();
	u32 allocatorSize1 = ((MallocAllocator*)(g_GlobalAllocators.pMalloc))->GetAllocatedMemorySize();
	// Move str1 into str2
	str2			   = (String&&)str1;
	u32 allocatorSize2 = ((MallocAllocator*)(g_GlobalAllocators.pMalloc))->GetAllocatedMemorySize();

	TEST_EQUAL(allocatorSize2, allocatorSize1 + strLength + 1);
}

TEST_CASE(StringTest, MoveStringWithSameAllocators)
{
	String str1("Hello World!!!!!!!!!!", g_GlobalAllocators.pMalloc);
	String str2(g_GlobalAllocators.pMalloc);

	u32 allocatorSize1 = ((MallocAllocator*)(g_GlobalAllocators.pMalloc))->GetAllocatedMemorySize();
	// Move str1 into str2
	str2			   = (String&&)str1;
	u32 allocatorSize2 = ((MallocAllocator*)(g_GlobalAllocators.pMalloc))->GetAllocatedMemorySize();

	TEST_EQUAL(allocatorSize2, allocatorSize1);
}