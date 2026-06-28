#include "core.h"
#include "tests/core/container_test_object.h"
#include "utilities/utilities.h"

using namespace ntt;

class QueueTest : public TestSuite
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

TEST_SUITE(QueueTest)

// ── Enqueue and Front ──

TEST_CASE(QueueTest, EnqueueAndFront)
{
	Queue<i32> queue;

	TEST_ASSERT(queue.IsEmpty());

	TEST_SUCCESS(queue.Enqueue(1));
	TEST_ASSERT(!queue.IsEmpty());
	TEST_EQUAL(queue.Front(), 1);

	TEST_SUCCESS(queue.Enqueue(2));
	TEST_EQUAL(queue.Front(), 1); // Front should still be 1 (FIFO)

	TEST_SUCCESS(queue.Enqueue(3));
	TEST_EQUAL(queue.Front(), 1);
}

TEST_CASE(QueueTest, FrontReadOnly)
{
	Queue<i32> queue;
	TEST_SUCCESS(queue.Enqueue(42));

	const auto& constRef = queue;
	TEST_EQUAL(constRef.Front(), 42);
}

TEST_CASE(QueueTest, FrontModification)
{
	Queue<i32> queue;
	TEST_SUCCESS(queue.Enqueue(10));

	queue.Front() = 99;
	TEST_EQUAL(queue.Front(), 99);
}

// ── Back peek ──

TEST_CASE(QueueTest, Back)
{
	Queue<i32> queue;
	TEST_SUCCESS(queue.Enqueue(1));
	TEST_EQUAL(queue.Back(), 1);

	TEST_SUCCESS(queue.Enqueue(2));
	TEST_EQUAL(queue.Back(), 2);

	TEST_SUCCESS(queue.Enqueue(3));
	TEST_EQUAL(queue.Back(), 3);

	// Front is still the first element
	TEST_EQUAL(queue.Front(), 1);
}

TEST_CASE(QueueTest, BackReadOnly)
{
	Queue<i32> queue;
	TEST_SUCCESS(queue.Enqueue(99));

	const auto& constRef = queue;
	TEST_EQUAL(constRef.Back(), 99);
}

TEST_CASE(QueueTest, BackModification)
{
	Queue<i32> queue;
	TEST_SUCCESS(queue.Enqueue(10));
	TEST_SUCCESS(queue.Enqueue(20));

	queue.Back() = 25;
	TEST_EQUAL(queue.Back(), 25);
	TEST_EQUAL(queue.Front(), 10); // Front unchanged
}

// ── Dequeue (FIFO order) ──

TEST_CASE(QueueTest, DequeueFIFO)
{
	Queue<i32> queue;
	TEST_SUCCESS(queue.Enqueue(1));
	TEST_SUCCESS(queue.Enqueue(2));
	TEST_SUCCESS(queue.Enqueue(3));

	TEST_EQUAL(queue.Front(), 1);
	TEST_SUCCESS(queue.Dequeue());
	TEST_EQUAL(queue.Front(), 2);

	TEST_SUCCESS(queue.Dequeue());
	TEST_EQUAL(queue.Front(), 3);

	TEST_SUCCESS(queue.Dequeue());
	TEST_ASSERT(queue.IsEmpty());
}

TEST_CASE(QueueTest, DequeueEmpty)
{
	Queue<i32> queue;
	TEST_EQUAL(queue.Dequeue(), RESULT_EMPTY_QUEUE);
}

// ── Move semantics ──

TEST_CASE(QueueTest, MoveConstructor)
{
	Queue<i32> queue1;
	TEST_SUCCESS(queue1.Enqueue(1));
	TEST_SUCCESS(queue1.Enqueue(2));

	Queue<i32> queue2((Queue<i32>&&)queue1);
	TEST_EQUAL(queue2.Front(), 1);
	TEST_ASSERT(queue1.IsEmpty());
}

// ── With TestObject ──

TEST_CASE(QueueTest, EnqueueWithObjects)
{
	Queue<TestObject> queue;

	TEST_SUCCESS(queue.Enqueue(TestObject()));
	TEST_SUCCESS(queue.Enqueue(TestObject()));
	TEST_SUCCESS(queue.Enqueue(TestObject()));

	TEST_ASSERT(!queue.IsEmpty());

	// 3 constructed, 3 temps destroyed, 0 copies, 3 moves
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 3);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

TEST_CASE(QueueTest, DequeueWithObjects)
{
	Queue<TestObject> queue;
	TEST_SUCCESS(queue.Enqueue(TestObject()));
	TEST_SUCCESS(queue.Enqueue(TestObject()));
	TEST_SUCCESS(queue.Enqueue(TestObject()));

	// Dequeue one element (destructor called inside ~Node)
	TEST_SUCCESS(queue.Dequeue());

	// 3 constructed, 3 temps + 1 dequeued = 4 destructors, 3 moves
	TEST_EQUAL(TestObject::s_ConstructorCount, 3);
	TEST_EQUAL(TestObject::s_DestructorCount, 4);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 3);
}

// ── Destructor cleanup ──

TEST_CASE(QueueTest, Destructor)
{
	{
		Queue<TestObject> queue;
		TEST_SUCCESS(queue.Enqueue(TestObject()));
		TEST_SUCCESS(queue.Enqueue(TestObject()));
		TEST_SUCCESS(queue.Enqueue(TestObject()));

		TEST_EQUAL(TestObject::s_ConstructorCount, 3);
		TEST_EQUAL(TestObject::s_DestructorCount, 3);
	}
	// After scope: ~Queue → ~List → Clear → 3 more destructors
	TEST_EQUAL(TestObject::s_DestructorCount, 6);
}

// ── Enqueue many ──

TEST_CASE(QueueTest, EnqueueMany)
{
	Queue<i32> queue;
	for (i32 i = 0; i < 100; ++i)
	{
		TEST_SUCCESS(queue.Enqueue(static_cast<i32&&>(i)));
	}
	TEST_EQUAL(queue.Front(), 0);
	TEST_EQUAL(queue.Back(), 99);
	TEST_ASSERT(!queue.IsEmpty());
}

// ── Enqueue then Dequeue all ──

TEST_CASE(QueueTest, EnqueueDequeueAll)
{
	Queue<i32> queue;
	for (i32 i = 0; i < 10; ++i)
	{
		TEST_SUCCESS(queue.Enqueue(static_cast<i32&&>(i)));
	}

	for (i32 i = 0; i < 10; ++i)
	{
		TEST_EQUAL(queue.Front(), i);
		TEST_SUCCESS(queue.Dequeue());
	}
	TEST_ASSERT(queue.IsEmpty());
}

// ── Interleave enqueue/dequeue ──

TEST_CASE(QueueTest, InterleaveEnqueueDequeue)
{
	Queue<i32> queue;

	TEST_SUCCESS(queue.Enqueue(1));
	TEST_SUCCESS(queue.Enqueue(2));
	TEST_SUCCESS(queue.Dequeue()); // removes 1
	TEST_EQUAL(queue.Front(), 2);

	TEST_SUCCESS(queue.Enqueue(3));
	TEST_EQUAL(queue.Front(), 2);

	TEST_SUCCESS(queue.Dequeue()); // removes 2
	TEST_EQUAL(queue.Front(), 3);

	TEST_SUCCESS(queue.Dequeue()); // removes 3
	TEST_ASSERT(queue.IsEmpty());
}

// ── Clear and reuse (via move) ──

TEST_CASE(QueueTest, MoveConstructorWithObjects)
{
	Queue<TestObject> queue1;
	TEST_SUCCESS(queue1.Enqueue(TestObject()));

	Queue<TestObject> queue2((Queue<TestObject>&&)queue1);

	TEST_ASSERT(queue1.IsEmpty());
	TEST_ASSERT(!queue2.IsEmpty());

	// 1 constructed, 1 temp destroyed, 1 move
	TEST_EQUAL(TestObject::s_ConstructorCount, 1);
	TEST_EQUAL(TestObject::s_DestructorCount, 1);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 1);
}
