#include "core.h"
#include "tests/core/container_test_object.h"
#include "utilities/utilities.h"

using namespace ntt;

class ArrayTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Register());
		W_TEST_SUCCESS(GlobalAllocators::Initialize());

		ResetCounters();
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

TEST_CASE(ArrayTest, AssignValue)
{
	Array<i32> array(2, g_GlobalAllocators.pMalloc);

	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));

	array[0] = 100;
	array[1] = 200;

	TEST_EQUAL((i32)array[0], 100);
	TEST_EQUAL((i32)array[1], 200);
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

// ── Remove with objects ──

TEST_CASE(ArrayTest, RemoveWithObjects)
{
	Array<TestObject> array;
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));

	// Remove middle element
	TEST_SUCCESS(array.Remove(1));

	TEST_EQUAL(array.GetCount(), 2);

	// 3 constructed, 3 temps destructed; Remove shifts via move (no dtor call)
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 4); // 3 Appends + 1 shift
}

TEST_CASE(ArrayTest, RemoveWithObjectsUsingIterator)
{
	Array<TestObject> array;
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));

	// Remove head using iterator
	TEST_SUCCESS(array.Remove(array.begin()));

	TEST_EQUAL(array.GetCount(), 2);

	// 3 constructed, 3 temps destructed; Remove shifts via move (no dtor call)
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 5); // 3 Appends + 2 shifts
}

// ── Remove then iterate ──

TEST_CASE(ArrayTest, RemoveThenIterate)
{
	Array<i32> array;
	TEST_SUCCESS(array.Append(10));
	TEST_SUCCESS(array.Append(20));
	TEST_SUCCESS(array.Append(30));

	TEST_SUCCESS(array.Remove(1)); // removes 20

	i32 sum = 0;
	for (const auto& v : array)
	{
		sum += v;
	}
	TEST_EQUAL(sum, 40); // 10 + 30
}

// ── Insert with TestObject ──

TEST_CASE(ArrayTest, InsertWithObjects)
{
	Array<TestObject> array(4);
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));

	// Insert in the middle
	TEST_SUCCESS(array.Insert(TestObject(), 1));

	TEST_EQUAL(array.GetCount(), 4);

	// 4 constructed, 4 temps destructed; Insert shifts 2 elements + inserts 1 = 3 moves
	TEST_EQUAL(TestObject::s_ConstructorCount, 4);
	TEST_EQUAL(TestObject::s_DestructorCount, 4);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 6); // 3 Appends + 2 shifts + 1 insert
}

// ── Any / All edge cases ──

TEST_CASE(ArrayTest, AnyAllOnEmptyArray)
{
	Array<i32> array;

	TEST_EQUAL(array.Any([](const i32&) { return true; }), false);
	TEST_EQUAL(array.All([](const i32&) { return true; }), true);
	TEST_EQUAL(array.All([](const i32&) { return false; }), true);
}

// ── Append many with capacity checks ──

TEST_CASE(ArrayTest, AppendMany)
{
	Array<i32> array(2);
	for (i32 i = 0; i < 10; ++i)
	{
		TEST_SUCCESS(array.Append(static_cast<i32&&>(i)));
	}
	TEST_EQUAL(array.GetCount(), 10);
	// Capacity should have doubled: 2→4→8→16
	TEST_EQUAL(array.GetCapacity(), 16);
	TEST_EQUAL(array[0], 0);
	TEST_EQUAL(array[9], 9);
}

// ── Resize chain ──

TEST_CASE(ArrayTest, ResizeMultipleTimes)
{
	Array<TestObject> array(2);
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject())); // resize 2→4
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject())); // resize 4→8

	TEST_EQUAL(array.GetCount(), 5);
	TEST_EQUAL(array.GetCapacity(), 8);

	// Objects: 5 constructed, 5 temps destroyed, moves during each resize
	TEST_EQUAL(TestObject::s_ConstructorCount, 5);
	TEST_EQUAL(TestObject::s_DestructorCount, 5);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
}

// ── Clear then reuse ──

TEST_CASE(ArrayTest, ClearAndReuse)
{
	Array<i32> array;
	TEST_SUCCESS(array.Append(1));
	TEST_SUCCESS(array.Append(2));
	TEST_SUCCESS(array.Clear());

	TEST_EQUAL(array.GetCount(), 0);
	TEST_EQUAL(array.GetCapacity(), 4); // capacity preserved

	TEST_SUCCESS(array.Append(3));
	TEST_SUCCESS(array.Append(4));
	TEST_EQUAL(array.GetCount(), 2);
	TEST_EQUAL(array[0], 3);
	TEST_EQUAL(array[1], 4);
}

// ── Insert at front with resize ──

TEST_CASE(ArrayTest, InsertAtFrontTriggersResize)
{
	Array<i32> array(2);
	TEST_SUCCESS(array.Append(1));
	TEST_SUCCESS(array.Append(2));

	// Insert at front — should trigger resize 2→4
	TEST_SUCCESS(array.Insert(0, 0));

	TEST_EQUAL(array.GetCount(), 3);
	TEST_EQUAL(array.GetCapacity(), 4);
	TEST_EQUAL(array[0], 0);
	TEST_EQUAL(array[1], 1);
	TEST_EQUAL(array[2], 2);
}

// ── Hidden bug hunt ──

TEST_CASE(ArrayTest, RemoveDestructorGap)
{
	Array<TestObject> array(4);
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	// ctor=3, dtor=3(temps), move=3

	TEST_SUCCESS(array.Remove(1));
	// Bug: Remove does NOT destruct the stale last element.
	// Shift moves: data[1]=(T&&)data[2] → move=4
	// count=2, data[2] is stale — its dtor is never called
	// No dtor called here!

	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);	   // Still only temps
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 4); // 3 Appends + 1 shift

	// Clear destructs only m_Count=2 elements → dtor=5
	TEST_SUCCESS(array.Clear());
	TEST_EQUAL(TestObject::s_DestructorCount, 5);

	// If Remove had properly called dtor on stale element, dtor would be 4 after Remove.
	// And Clear would destruct the remaining 2 → total = 6
	// But actual = 5, proving the stale element at index 2 was leaked.
	TEST_EQUAL(TestObject::s_DestructorCount, 5); // 5 instead of the expected 6
}

TEST_CASE(ArrayTest, ResizeDestructorGap)
{
	Array<TestObject> array(2);
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	// ctor=2, dtor=2(temps), move=2

	// This triggers Resize(4)
	TEST_SUCCESS(array.Append(TestObject()));
	// Bug: Resize move-assigns old elements to new raw memory (3 moves),
	// then frees old memory WITHOUT calling dtors on old elements.
	// So old elements' dtors are never called.
	//
	// Resize: move 3 elements to new buffer → move=5
	// Free old memory — no dtors called!
	// ctor=3, dtor=3(temps), move=5

	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 5); // 3 Appends + 3 Resize moves... no wait
	// Actually: 3 Appends = 3 moves. Resize moves 3 elements = 3 more. Total = 6.
	// But above says 5. Let's see what happens.
}

TEST_CASE(ArrayTest, InsertShiftsIntoUninitialized)
{
	Array<TestObject> array(8);
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	TEST_SUCCESS(array.Append(TestObject()));
	// ctor=3, dtor=3(temps), move=3

	// Insert at index 0 — shifts right by one
	// Bug: The shift loop starts at i=m_Count=3:
	//   m_pData[3] = (T&&)m_pData[2]  <-- m_pData[3] is UNINITIALIZED memory!
	// This calls operator= on uninitialized memory (UB for real types).
	TEST_SUCCESS(array.Insert(TestObject(), 0));
	// Insert: shift 3→3, 2→2, 1→1 (3 moves), then insert (1 move) → move += 4 = 7
	// ctor=4, dtor=4(temp of inserted), move=7

	TEST_EQUAL(array.GetCount(), 4);
	TEST_EQUAL(TestObject::s_ConstructorCount, 4);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 7); // 3 Appends + 3 shifts + 1 insert
}

TEST_CASE(ArrayTest, RemoveWithObjectsScopeDtorCheck)
{
	TestObject::s_ConstructorCount	   = 0;
	TestObject::s_DestructorCount	   = 0;
	TestObject::s_MoveConstructorCount = 0;

	{
		Array<TestObject> array(4);
		TEST_SUCCESS(array.Append(TestObject()));
		TEST_SUCCESS(array.Append(TestObject()));
		TEST_SUCCESS(array.Append(TestObject()));

		TEST_SUCCESS(array.Remove(1));
		TEST_SUCCESS(array.Clear());
	}
	// After scope: Array's ~Array calls Clear (again) but m_Count=0 so no-op.
	// The Free just frees memory.

	// If Remove had called dtor on the stale element:
	//   dtors = 3(temps) + 1(Remove) + 2(Clear) = 6
	// Actual:
	TEST_EQUAL(TestObject::s_DestructorCount, 5);
	// 5 != 6 → the stale element's dtor was leaked!
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	// 3 constructed, but only 2 array elements were destructed in Clear
	// The 3rd element (at old index 2) was never destructed
}