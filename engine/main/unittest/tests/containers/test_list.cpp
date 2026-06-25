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

TEST_CASE(ListTest, InsertBefore)
{
	List<i32> list;

	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	auto it = list.Begin();
	++it; // Move to the second element (3)

	TEST_SUCCESS(list.InsertBefore(it, 1)); // Insert 1 before 3

	TEST_EQUAL(list.GetCount(), 3);
	TEST_EQUAL(list[0], 2);
	TEST_EQUAL(list[1], 1);
	TEST_EQUAL(list[2], 3);

	list.Clear();

	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	it = list.Begin(); // Iterator pointing to the first element (2)

	TEST_SUCCESS(list.InsertBefore(it, 1)); // Insert 1 before 2

	TEST_EQUAL(list.GetCount(), 3);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 2);
	TEST_EQUAL(list[2], 3);

	list.Clear();

	TEST_SUCCESS(list.InsertBefore(list.Begin(), 1)); // Insert 1 before the first element (which is nullptr)
	TEST_EQUAL(list.GetCount(), 1);
	TEST_EQUAL(list[0], 1);
}

// ── InsertAfter edge cases ──

TEST_CASE(ListTest, InsertAfterSingleElement)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));

	// Insert after the only element (head = tail)
	TEST_SUCCESS(list.InsertAfter(list.Begin(), 2));

	TEST_EQUAL(list.GetCount(), 2);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 2);
}

TEST_CASE(ListTest, InsertAfterChain)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));

	// Chain: insert 2 after 1, then 3 after 2
	auto it = list.Begin();
	TEST_SUCCESS(list.InsertAfter(it, 2));
	++it; // now points to 2
	TEST_SUCCESS(list.InsertAfter(it, 3));

	TEST_EQUAL(list.GetCount(), 3);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 2);
	TEST_EQUAL(list[2], 3);
}

TEST_CASE(ListTest, InsertAfterWithObjects)
{
	List<TestObject> list;

	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));

	auto it = list.Begin();
	++it;

	TEST_SUCCESS(list.InsertAfter(list.Begin(), TestObject()));

	TEST_EQUAL(list.GetCount(), 3);

	// 3 temporaries constructed, destroyed; 0 copies; 3 moves into the list
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

// ── InsertBefore edge cases ──

TEST_CASE(ListTest, InsertBeforeSingleElement)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));

	// Insert before the only element
	TEST_SUCCESS(list.InsertBefore(list.Begin(), 0));

	TEST_EQUAL(list.GetCount(), 2);
	TEST_EQUAL(list[0], 0);
	TEST_EQUAL(list[1], 1);
}

TEST_CASE(ListTest, InsertBeforeChain)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(3));

	// Chain: insert 2 before 3, then 1 before 2
	auto it = list.Begin(); // points to 3
	TEST_SUCCESS(list.InsertBefore(it, 2));
	it = list.Begin(); // points to 2 now
	TEST_SUCCESS(list.InsertBefore(it, 1));

	TEST_EQUAL(list.GetCount(), 3);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 2);
	TEST_EQUAL(list[2], 3);
}

TEST_CASE(ListTest, InsertBeforeWithObjects)
{
	List<TestObject> list;

	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));

	// Insert before the head
	TEST_SUCCESS(list.InsertBefore(list.Begin(), TestObject()));

	TEST_EQUAL(list.GetCount(), 3);

	// 3 temporaries constructed, destroyed; 0 copies; 3 moves into the list
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

// ── Remove edge cases ──

TEST_CASE(ListTest, RemoveFromEmptyList)
{
	List<i32> list;

	// Removing from an empty list should fail
	TEST_EQUAL(list.Remove(list.End()), RESULT_INVALID_ITERATOR);
	TEST_EQUAL(list.GetCount(), 0);
}

TEST_CASE(ListTest, RemoveOnlyElement)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(42));

	TEST_SUCCESS(list.Remove(list.Begin()));
	TEST_EQUAL(list.GetCount(), 0);
}

TEST_CASE(ListTest, RemoveHead)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	// Remove the head (1)
	TEST_SUCCESS(list.Remove(list.Begin()));
	TEST_EQUAL(list.GetCount(), 2);
	TEST_EQUAL(list[0], 2);
	TEST_EQUAL(list[1], 3);
}

TEST_CASE(ListTest, RemoveTail)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	// Remove the tail (3) — iterate to last element
	auto it = list.Begin();
	++it; // 2
	++it; // 3
	TEST_SUCCESS(list.Remove(it));
	TEST_EQUAL(list.GetCount(), 2);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 2);
}

TEST_CASE(ListTest, RemoveNullIterator)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));

	// Attempt to remove using a null iterator (End())
	TEST_EQUAL(list.Remove(list.End()), RESULT_INVALID_ITERATOR);
	TEST_EQUAL(list.GetCount(), 2);
}

TEST_CASE(ListTest, RemoveMiddle)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	// Remove the middle element (2)
	auto it = list.Begin();
	++it; // now points to 2
	TEST_SUCCESS(list.Remove(it));
	TEST_EQUAL(list.GetCount(), 2);
	TEST_EQUAL(list[0], 1);
	TEST_EQUAL(list[1], 3);
}

TEST_CASE(ListTest, RemoveAllSequentially)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));

	// Remove head repeatedly until empty
	TEST_SUCCESS(list.Remove(list.Begin())); // removes 1
	TEST_EQUAL(list.GetCount(), 2);
	TEST_EQUAL(list[0], 2);

	TEST_SUCCESS(list.Remove(list.Begin())); // removes 2
	TEST_EQUAL(list.GetCount(), 1);
	TEST_EQUAL(list[0], 3);

	TEST_SUCCESS(list.Remove(list.Begin())); // removes 3
	TEST_EQUAL(list.GetCount(), 0);
}

TEST_CASE(ListTest, RemoveWithObjects)
{
	List<TestObject> list;
	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));
	TEST_SUCCESS(list.Append(TestObject()));

	// Remove the middle element
	auto it = list.Begin();
	++it;
	TEST_SUCCESS(list.Remove(it));

	TEST_EQUAL(list.GetCount(), 2);

	// 3 constructed (Append temps), 1 destroyed (the removed node via ~Node),
	// plus 3 temps destroyed = 4 total destructors
	// 0 copies, 3 moves (moved into list via Append)
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 4);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

TEST_CASE(ListTest, RemoveAndIterate)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Append(2));
	TEST_SUCCESS(list.Append(3));
	TEST_SUCCESS(list.Append(4));

	// Remove middle (2), then iterate to verify links
	auto it = list.Begin();
	++it;
	TEST_SUCCESS(list.Remove(it));

	i32 expected[] = {1, 3, 4};
	i32 index	   = 0;
	for (auto iter = list.Begin(); iter != list.End(); ++iter)
	{
		TEST_EQUAL(*iter, expected[index]);
		index++;
	}
	TEST_EQUAL(index, 3);
}

TEST_CASE(ListTest, RemoveThenAppend)
{
	List<i32> list;
	TEST_SUCCESS(list.Append(1));
	TEST_SUCCESS(list.Remove(list.Begin()));

	// Append again into the now-empty list
	TEST_SUCCESS(list.Append(2));
	TEST_EQUAL(list.GetCount(), 1);
	TEST_EQUAL(list[0], 2);
}