#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class TestObject
{
public:
	static i32 s_ConstructorCount;
	static i32 s_DestructorCount;
	static i32 s_CopyConstructorCount;
	static i32 s_MoveConstructorCount;

public:
	TestObject(i32 value = 0)
		: m_Value(value)
	{
		s_ConstructorCount++;
	}

	TestObject(const TestObject& other)
		: m_Value(other.m_Value)
	{
		s_CopyConstructorCount++;
	}

	TestObject(TestObject&& other) noexcept
		: m_Value(other.m_Value)
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

	operator i32() const
	{
		return m_Value;
	}

private:
	i32 m_Value;
};

i32 TestObject::s_ConstructorCount	   = 0;
i32 TestObject::s_DestructorCount	   = 0;
i32 TestObject::s_CopyConstructorCount = 0;
i32 TestObject::s_MoveConstructorCount = 0;

class ArrayTest : public TestSuite
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

TEST_SUITE(ArrayTest)

TEST_CASE(ArrayTest, AppendElements)
{
	Array<i32> array(2, g_GlobalAllocators.pMalloc);

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

TEST_CASE(ArrayTest, ResizeArray)
{
	Array<TestObject> array(2); // Start with capacity 2

	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS); // This should trigger a resize
	TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS); // This should trigger another resize

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4); // Capacity should have doubled from 2 to 4

	// Resize to a larger capacity
	TEST_EQUAL(array.Resize(8), RESULT_SUCCESS);
	TEST_EQUAL(array.GetCapacity(), 8);

	TEST_EQUAL(array.GetCount(), 3); // Count should remain the same after resizing
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3); // No additional destructors
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 8); // Move constructors called during

	// Resize to a smaller capacity (should fail)
	TEST_EQUAL(array.Resize(2), RESULT_NEW_CAPACITY_TOO_SMALL);

	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3); // No additional destructors
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 8); // Move constructors called during
}

TEST_CASE(ArrayTest, IndexOperator)
{
	Array<i32> array;

	TEST_EQUAL(array.Append(10), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(20), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(30), RESULT_SUCCESS);

	TEST_EQUAL((i32)array[0], 10);
	TEST_EQUAL((i32)array[1], 20);
	TEST_EQUAL((i32)array[2], 30);

	// Test out-of-bounds access (should assert)
	// Uncommenting the following line will trigger an assertion failure
	// i32 value = array[3];
}

TEST_CASE(ArrayTest, MoveConstructor)
{
	Array<TestObject> array1;

	TEST_EQUAL(array1.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array1.Append(TestObject()), RESULT_SUCCESS);

	TEST_EQUAL(array1.GetCount(), 2);

	// Move construct array2 from array1
	Array<TestObject> array2((Array<TestObject>&&)array1);

	TEST_EQUAL(array2.GetCount(), 2);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 2);
	TEST_EQUAL(TestObject::s_DestructorCount, 2); // No destructors called yet
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 2); // Move constructors called during move
}

TEST_CASE(ArrayTest, MoveAssignmentOperator)
{
	Array<TestObject> array1;

	TEST_EQUAL(array1.Append(TestObject()), RESULT_SUCCESS);
	TEST_EQUAL(array1.Append(TestObject()), RESULT_SUCCESS);

	TEST_EQUAL(array1.GetCount(), 2);

	// Move assign array2 from array1
	Array<TestObject> array2;
	array2 = (Array<TestObject>&&)array1;

	TEST_EQUAL(array2.GetCount(), 2);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 2);
	TEST_EQUAL(TestObject::s_DestructorCount, 2); // No destructors called yet
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 2); // Move constructors called during move
}

TEST_CASE(ArrayTest, Destructor)
{
	{
		Array<TestObject> array;

		TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
		TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);
		TEST_EQUAL(array.Append(TestObject()), RESULT_SUCCESS);

		TEST_EQUAL(array.GetCount(), 3);

		// Check constructor and destructor counts
		TEST_EQUAL(TestObject::s_ConstructorCount, 3);
		TEST_EQUAL(TestObject::s_DestructorCount, 3); // No destructors called yet
		TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
		TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
	}

	// After the array goes out of scope, destructors should be called
	TEST_EQUAL(TestObject::s_DestructorCount, 6);
}

TEST_CASE(ArrayTest, FindIndex)
{
	Array<i32> array;

	TEST_EQUAL(array.Append(10), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(20), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(30), RESULT_SUCCESS);

	auto predicate = [](const i32& value) -> bool { return value == 20; };
	i32	 index	   = array.FindIndex(predicate);

	TEST_EQUAL(index, 1); // The index of the value 20 should be 1

	auto notFoundPredicate = [](const i32& value) -> bool { return value == 40; };
	i32	 notFoundIndex	   = array.FindIndex(notFoundPredicate);

	TEST_EQUAL(notFoundIndex, -1); // The value 40 is not in the array, so it should return -1
}

TEST_CASE(ArrayTest, IsEmptyAndAnyAll)
{
	Array<i32> array;

	TEST_EQUAL(array.IsEmpty(), true);

	TEST_EQUAL(array.Append(10), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(20), RESULT_SUCCESS);
	TEST_EQUAL(array.Append(30), RESULT_SUCCESS);

	TEST_EQUAL(array.IsEmpty(), false);

	TEST_EQUAL(array.Any([](const i32& value) -> bool { return value == 20; }), true);
	TEST_EQUAL(array.Any([](const i32& value) { return value >= 20; }), true);
	TEST_EQUAL(array.Any([](const i32& value) { return value >= 40; }), false);
	TEST_EQUAL(array.All([](const i32& value) -> bool { return value > 0; }), true);
	TEST_EQUAL(array.All([](const i32& value) -> bool { return value < 30; }), false);
}