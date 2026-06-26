#include "container_test_object.h"
#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class StackTest : public TestSuite
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

TEST_SUITE(StackTest)

// ── Push and Top ──

TEST_CASE(StackTest, PushAndTop)
{
	Stack<i32> stack;

	TEST_ASSERT(stack.IsEmpty());

	TEST_SUCCESS(stack.Push(1));
	TEST_ASSERT(!stack.IsEmpty());
	TEST_EQUAL(stack.Top(), 1);

	TEST_SUCCESS(stack.Push(2));
	TEST_EQUAL(stack.Top(), 2);

	TEST_SUCCESS(stack.Push(3));
	TEST_EQUAL(stack.Top(), 3);
}

TEST_CASE(StackTest, TopReadOnly)
{
	Stack<i32> stack;
	TEST_SUCCESS(stack.Push(42));

	const auto& constRef = stack;
	TEST_EQUAL(constRef.Top(), 42);
}

TEST_CASE(StackTest, TopModification)
{
	Stack<i32> stack;
	TEST_SUCCESS(stack.Push(10));

	stack.Top() = 99;
	TEST_EQUAL(stack.Top(), 99);
}

// ── Move semantics ──

TEST_CASE(StackTest, MoveConstructor)
{
	Stack<i32> stack1;
	TEST_SUCCESS(stack1.Push(1));
	TEST_SUCCESS(stack1.Push(2));

	Stack<i32> stack2((Stack<i32>&&)stack1);
	TEST_EQUAL(stack2.Top(), 2);
	TEST_ASSERT(stack1.IsEmpty());
}

// ── Move constructor with objects ──

TEST_CASE(StackTest, MoveConstructorWithObjects)
{
	Stack<TestObject> stack1;
	TEST_SUCCESS(stack1.Push(TestObject()));

	Stack<TestObject> stack2((Stack<TestObject>&&)stack1);

	TEST_ASSERT(stack1.IsEmpty());
	TEST_ASSERT(!stack2.IsEmpty());

	// 1 constructed, 1 temp destroyed, 1 move (into stack1)
	TEST_EQUAL(TestObject::s_ConstructorCount, 1);
	TEST_EQUAL(TestObject::s_DestructorCount, 1);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 1);
}

// ── With TestObject ──

TEST_CASE(StackTest, PushWithObjects)
{
	Stack<TestObject> stack;

	TEST_SUCCESS(stack.Push(TestObject()));
	TEST_SUCCESS(stack.Push(TestObject()));
	TEST_SUCCESS(stack.Push(TestObject()));

	TEST_ASSERT(!stack.IsEmpty());

	// 3 constructed, 3 temps destroyed, 0 copies, 3 moves
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

// ── Destructor cleanup ──

TEST_CASE(StackTest, Destructor)
{
	{
		Stack<TestObject> stack;
		TEST_SUCCESS(stack.Push(TestObject()));
		TEST_SUCCESS(stack.Push(TestObject()));
		TEST_SUCCESS(stack.Push(TestObject()));

		TEST_EQUAL(TestObject::s_ConstructorCount, 3);
		TEST_EQUAL(TestObject::s_DestructorCount, 3);
	}
	// After scope: ~Stack → ~List → Clear → 3 more destructors
	TEST_EQUAL(TestObject::s_DestructorCount, 6);
}

// ── Push many ──

TEST_CASE(StackTest, PushMany)
{
	Stack<i32> stack;
	for (i32 i = 0; i < 100; ++i)
	{
		TEST_SUCCESS(stack.Push(static_cast<i32&&>(i)));
	}
	TEST_EQUAL(stack.Top(), 99);
	TEST_ASSERT(!stack.IsEmpty());
}

// ── Multiple pushes verify Top ──

TEST_CASE(StackTest, PushRepeatedTop)
{
	Stack<i32> stack;

	for (i32 i = 0; i < 10; ++i)
	{
		TEST_SUCCESS(stack.Push(static_cast<i32&&>(i)));
		TEST_EQUAL(stack.Top(), i);
	}
	TEST_ASSERT(!stack.IsEmpty());
}

// ── Top on const ref ──

TEST_CASE(StackTest, ConstTop)
{
	const Stack<i32> emptyStack;
	// Can't call Top on empty stack (asserts), but should compile
	// Stack has const Top() overload

	Stack<i32> stack;
	TEST_SUCCESS(stack.Push(7));
	const auto& ref = stack;
	TEST_EQUAL(ref.Top(), 7);
}