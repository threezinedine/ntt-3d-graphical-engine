#include "container_test_object.h"
#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class ListTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Register());
		W_TEST_SUCCESS(GlobalAllocators::Initialize());

		TestObject::s_ConstructorCount	   = 0;
		TestObject::s_DestructorCount	   = 0;
		TestObject::s_CopyConstructorCount = 0;
		TestObject::s_MoveConstructorCount = 0;
	}

	ON_AFTER_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Shutdown());
		W_TEST_SUCCESS(GlobalAllocators::Unregister());
	}
};

TEST_SUITE(ListTest)

TEST_CASE(ListTest, AppendElements)
{
	List<i32> list;

	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	TEST_EQUAL(list.GetCount(), 3);
}

TEST_CASE(ListTest, AppendElement)
{
	List<TestObject> list;

	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));

	TEST_EQUAL(list.GetCount(), 3);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3); // All temporary objects should be destructed
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

TEST_CASE(ListTest, ClearList)
{
	List<TestObject> list;

	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));

	TEST_EQUAL(list.GetCount(), 3);

	TEST_SUCCESS(list.Clear());
	TEST_EQUAL(list.GetCount(), 0);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 6); // All objects should be destructed
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

TEST_CASE(ListTest, Iterator)
{
	List<i32> list;

	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	i32 expectedValue = 1;
	for (auto it = list.Begin(); it != list.End(); ++it)
	{
		TEST_EQUAL(*it, expectedValue);
		expectedValue++;
	}

	TEST_EQUAL(expectedValue, 4); // Ensure we iterated through all elements
}

TEST_CASE(ListTest, InsertAfter)
{
	List<i32> list;

	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(3));

	auto it = list.Begin();
	++it; // Move to the second element (3)

	TEST_SUCCESS(list.InsertAfter(it, 2)); // Insert 2 after 3

	TEST_EQUAL(list.GetCount(), 3);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 3);
	TEST_EQUAL(list[2], 2);

	list.Clear();

	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));

	it = list.End(); // Iterator pointing to the end (nullptr)

	TEST_SUCCESS(list.InsertAfter(it, 3)); // Insert 3 at the end

	TEST_EQUAL(list.GetCount(), 3);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 2);
	TEST_EQUAL(list[2], 3);

	list.Clear();

	TEST_SUCCESS(list.InsertAfter(list.Begin(), 1)); // Insert 1 after the first element (which is nullptr)
	TEST_EQUAL(list.GetCount(), 1);
	TEST_EQUAL(list[0], 1);
}