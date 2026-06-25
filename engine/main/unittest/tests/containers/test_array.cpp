#include "container_test_object.h"
#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

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

	TEST_SUCCESS(array.Append(1));
	TEST_SUCCESS(array.Append(2));
	TEST_SUCCESS(array.Append(3)); // This should trigger a resize

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4); // Capacity should have doubled from 2 to
}

TEST_CASE(ArrayTest, AppendObjects)
{
	Array<TestObject> array;

	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject())); // This should trigger a resize

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

	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject())); // This should trigger a resize
	TEST_SUCCESS(array.Append(TestObject())); // This should trigger another resize

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

	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));

	TEST_EQUAL(array.GetCount(), 3);

	TEST_SUCCESS(array.Clear());
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

	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject())); // This should trigger a resize
	TEST_SUCCESS(array.Append(TestObject())); // This should trigger another resize

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4); // Capacity should have doubled from 2 to 4

	// Resize to a larger capacity
	TEST_SUCCESS(array.Resize(8));
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

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

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

	TEST_SUCCESS(array1.Append(TestObject()));
	TEST_SUCCESS(array1.Append(TestObject()));

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

	TEST_SUCCESS(array1.Append(TestObject()));
	TEST_SUCCESS(array1.Append(TestObject()));

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

		TEST_SUCCESS(array.Append(TestObject()));
		TEST_SUCCESS(array.Append(TestObject()));
		TEST_SUCCESS(array.Append(TestObject()));

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

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	TEST_EQUAL(array.FindIndex([](const i32& value) -> bool { return value == 20; }), 1);

	TEST_EQUAL(array.FindIndex([](const i32& value) -> bool { return value == 40; }), -1);
}

TEST_CASE(ArrayTest, Find)
{
	Array<i32> array;

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	Array<i32>::Iterator iterator = array.Find([](const i32& value) -> bool { return value == 20; });

	TEST_ASSERT(iterator != array.end()); // The iterator should not be equal to end()
	TEST_EQUAL(*iterator, 20);			  // The value pointed to by the iterator should be 20

	Array<i32>::Iterator notFoundIterator = array.Find([](const i32& value) -> bool { return value == 40; });

	TEST_ASSERT(notFoundIterator == array.end()); // The iterator should be equal to end() since the value is not found
}

TEST_CASE(ArrayTest, OperatorForLoop)
{
	Array<i32> array;

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	i32 expectedValue = 0;
	for (const auto& value : array)
	{
		expectedValue += value;
	}

	TEST_EQUAL(expectedValue, 60); // After the loop, expectedValue should be 60
}

TEST_CASE(ArrayTest, IsEmptyAndAnyAll)
{
	Array<i32> array;

	TEST_EQUAL(array.IsEmpty(), true);

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	TEST_EQUAL(array.IsEmpty(), false);

	TEST_EQUAL(array.Any([](const i32& value) -> bool { return value == 20; }), true);
	TEST_EQUAL(array.Any([](const i32& value) { return value >= 20; }), true);
	TEST_EQUAL(array.Any([](const i32& value) { return value >= 40; }), false);
	TEST_EQUAL(array.All([](const i32& value) -> bool { return value > 0; }), true);
	TEST_EQUAL(array.All([](const i32& value) -> bool { return value < 30; }), false);
}

TEST_CASE(ArrayTest, InsertElements)
{
	Array<i32> array;

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(30));

	TEST_SUCCESS(array.Insert(20, 1)); // Insert 20 at index 1

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL((i32)array[0], 10);
	TEST_EQUAL((i32)array[1], 20);
	TEST_EQUAL((i32)array[2], 30);

	// Test inserting at the beginning
	TEST_SUCCESS(array.Insert(5, 0)); // Insert 5 at index 0

	TEST_EQUAL(array.GetCount(), 4);
	TEST_EQUAL((i32)array[0], 5);
	TEST_EQUAL((i32)array[1], 10);
	TEST_EQUAL((i32)array[2], 20);
	TEST_EQUAL((i32)array[3], 30);

	// Test inserting at the end
	TEST_SUCCESS(array.Insert(40, array.GetCount())); // Insert 40 at the end

	TEST_EQUAL(array.GetCount(), 5);
	TEST_EQUAL((i32)array[4], 40);

	// Test inserting out of bounds
	TEST_EQUAL(array.Insert(50, array.GetCount() + 1), RESULT_INDEX_OUT_OF_BOUNDS); // Out of bounds
}

TEST_CASE(ArrayTest, InsertUsingIterator)
{
	Array<i32> array;

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(30));

	auto iter = array.begin();
	++iter; // Move iterator to index 1

	TEST_SUCCESS(array.Insert(20, iter)); // Insert 20 at index 1 using iterator

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL((i32)array[0], 10);
	TEST_EQUAL((i32)array[1], 20);
	TEST_EQUAL((i32)array[2], 30);

	TEST_SUCCESS(array.Insert(5, array.end())); // Insert 5 at the end using iterator
	TEST_EQUAL(array.GetCount(), 4);
	TEST_EQUAL((i32)array[3], 5);
}

TEST_CASE(ArrayTest, RemoveElements)
{
	Array<i32> array;

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	TEST_SUCCESS(array.Remove(1)); // Remove element at index 1 (value 20)

	TEST_EQUAL(array.GetCount(), 2);
	TEST_EQUAL((i32)array[0], 10);
	TEST_EQUAL((i32)array[1], 30);

	// Test removing the first element
	TEST_SUCCESS(array.Remove(0)); // Remove element at index 0 (value 10)

	TEST_EQUAL(array.GetCount(), 1);
	TEST_EQUAL((i32)array[0], 30);

	// Test removing the last element
	TEST_SUCCESS(array.Remove(0)); // Remove element at index 0 (value 30)

	TEST_EQUAL(array.GetCount(), 0);

	// Test removing from an empty array
	TEST_EQUAL(array.Remove(0), RESULT_INDEX_OUT_OF_BOUNDS); // Out of bounds
}

TEST_CASE(ArrayTest, RemoveUsingIterator)
{
	Array<i32> array;

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	Array<i32>::Iterator iter = array.begin();
	++iter; // Move iterator to index 1 (value 20)

	TEST_SUCCESS(array.Remove(iter)); // Remove element at index 1 using iterator

	TEST_EQUAL(array.GetCount(), 2);
	TEST_EQUAL((i32)array[0], 10);
	TEST_EQUAL((i32)array[1], 30);

	// Test removing the first element using iterator
	iter = array.begin();			  // Move iterator to index 0 (value 10)
	TEST_SUCCESS(array.Remove(iter)); // Remove element at index 0 using iterator

	TEST_EQUAL(array.GetCount(), 1);
	TEST_EQUAL((i32)array[0], 30);

	// Test removing the last element using iterator
	iter = array.begin();			  // Move iterator to index 0 (value 30)
	TEST_SUCCESS(array.Remove(iter)); // Remove element at index 0 using iterator

	TEST_EQUAL(array.GetCount(), 0);

	// Test removing from an empty array using iterator
	iter = array.begin();										// Iterator points to end() since the array is empty
	TEST_EQUAL(array.Remove(iter), RESULT_INDEX_OUT_OF_BOUNDS); // Out of bounds
}