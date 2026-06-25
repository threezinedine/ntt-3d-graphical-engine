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

	TEST_EQUAL(list.Append(1), RESULT_SUCCESS);
	TEST_EQUAL(list.Append(2), RESULT_SUCCESS);
	TEST_EQUAL(list.Append(3), RESULT_SUCCESS);

	TEST_EQUAL(list.GetCount(), 3);
}

TEST_CASE(ListTest, AppendElement)
{
	List<TestObject> list;

	TEST_EQUAL(list.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(list.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(list.Append(TestObject()), RESULT_SUCCESS);

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

	TEST_EQUAL(list.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(list.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(list.Append(TestObject()), RESULT_SUCCESS);

	TEST_EQUAL(list.GetCount(), 3);

	TEST_EQUAL(list.Clear(), RESULT_SUCCESS);
	TEST_EQUAL(list.GetCount(), 0);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 6); // All objects should be destructed
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}