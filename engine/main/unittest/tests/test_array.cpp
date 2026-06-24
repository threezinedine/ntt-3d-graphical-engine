#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class TestObject
{
public:
	static int s_ConstructorCount;
	static int s_DestructorCount;
	static int s_CopyConstructorCount;
	static int s_MoveConstructorCount;

public:
	TestObject()
	{
		s_ConstructorCount++;
	}

	TestObject(const TestObject&)
	{
		s_CopyConstructorCount++;
	}

	TestObject(TestObject&&) noexcept
	{
		s_MoveConstructorCount++;
	}

	~TestObject()
	{
		s_DestructorCount++;
	}

	void operator=(TestObject&&) noexcept
	{
		s_MoveConstructorCount++;
	}
};

int TestObject::s_ConstructorCount	   = 0;
int TestObject::s_DestructorCount	   = 0;
int TestObject::s_CopyConstructorCount = 0;
int TestObject::s_MoveConstructorCount = 0;

class ArrayTest : public TestSuite
{
	void OnBeforeEachTestImpl() override
	{
		GlobalAllocators::Register();
		GlobalAllocators::Initialize();
		TestObject::s_ConstructorCount	   = 0;
		TestObject::s_DestructorCount	   = 0;
		TestObject::s_CopyConstructorCount = 0;
		TestObject::s_MoveConstructorCount = 0;
	}

	void OnAfterEachTestImpl() override
	{
		GlobalAllocators::Shutdown();
		GlobalAllocators::Unregister();
	}
};

TEST_SUITE(ArrayTest)

TEST_CASE(ArrayTest, AppendElements)
{
	Array<int> array(2, g_GlobalAllocators.pMalloc);

	TEST_EQUAL(array.Append(1), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(2), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(3), RESULT_SUCCESS); // This should trigger a resize

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4); // Capacity should have doubled from 2 to
}

TEST_CASE(ArrayTest, AppendObjects)
{
	Array<TestObject> array;

	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS); // This should trigger a resize

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4); // Capacity should have doubled from 2 to 4

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3); // All temporary objects should be destructed
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

TEST_CASE(ArrayTest, AppendAndResizeObjects)
{
	Array<TestObject> array(2); // Start with capacity 2

	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS); // This should trigger a resize
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS); // This should trigger another resize

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4); // Capacity should have doubled from 2 to 4

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3); // All temporary objects should be destructed
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 5);
}

TEST_CASE(ArrayTest, ClearArray)
{
	Array<TestObject> array;

	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);

	TEST_EQUAL(array.GetCount(), 3);

	TEST_EQUAL(array.Clear(), RESULT_SUCCESS);
	TEST_EQUAL(array.GetCount(), 0);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 6); // All objects should be destructed
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}