#include "containers/container_test_object.h"
#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class SmartPtrTest : public TestSuite
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

TEST_SUITE(SmartPtrTest)

TEST_CASE(SmartPtrTest, ScopeSmartPtr)
{
	Scope<i32> smartPtr = MakeScope<i32>(g_GlobalAllocators.pMalloc, 42);
	TEST_EQUAL(*smartPtr.Get(), 42);

	Scope<i32> movedSmartPtr = move(smartPtr);
	TEST_EQUAL(*movedSmartPtr.Get(), 42);
	TEST_EQUAL(smartPtr.Get(), nullptr); // smartPtr should be null after move
}

TEST_CASE(SmartPtrTest, ScopeSmartPtrWithAllocator)
{
	{
		Scope<TestObject> smartPtr = MakeScope<TestObject>(g_GlobalAllocators.pMalloc, 100);
		TEST_EQUAL(static_cast<i32>(*(smartPtr.Get())), 100);

		Scope<TestObject> movedSmartPtr = move(smartPtr);
		TEST_EQUAL(static_cast<i32>(*(movedSmartPtr.Get())), 100);
		TEST_EQUAL(smartPtr.Get(), nullptr); // smartPtr should be null after move
	}

	TEST_EQUAL(TestObject::s_ConstructorCount, 1);
	TEST_EQUAL(TestObject::s_DestructorCount, 1);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 0);
}

TEST_CASE(SmartPtrTest, TransferTheAllocator)
{
	{
		Scope<TestObject> smartPtr1 = MakeScope<TestObject>(g_GlobalAllocators.pMalloc, 200);
		Scope<TestObject> smartPtr2 = MakeScope<TestObject>(g_GlobalAllocators.pStack, 300);

		// Move smartPtr1 to smartPtr2, transferring the allocator
		smartPtr2 = move(smartPtr1);

		TEST_EQUAL(static_cast<i32>(*smartPtr2.Get()), 200);
		TEST_EQUAL(smartPtr1.Get(), nullptr); // smartPtr1 should be null after move

		// Move smartPtr2 back to smartPtr1, transferring the allocator again
		smartPtr1 = move(smartPtr2);

		TEST_EQUAL(static_cast<i32>(*smartPtr1.Get()), 200);
		TEST_EQUAL(smartPtr2.Get(), nullptr); // smartPtr2 should be null after move
	}

	TEST_EQUAL(TestObject::s_ConstructorCount, 2); // Two TestObject instances were created
	TEST_EQUAL(TestObject::s_DestructorCount, 2);  // Both instances should be destroyed at the end of the scope
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 0);
}

TEST_CASE(SmartPtrTest, ResetSmartPtr)
{
	Scope<TestObject> smartPtr = MakeScope<TestObject>(g_GlobalAllocators.pMalloc, 400);
	TEST_EQUAL(static_cast<i32>(*smartPtr.Get()), 400);

	smartPtr.Reset();
	TEST_EQUAL(smartPtr.Get(), nullptr); // smartPtr should be null after reset

	// After reset, the destructor should have been called
	TEST_EQUAL(TestObject::s_ConstructorCount, 1);
	TEST_EQUAL(TestObject::s_DestructorCount, 1);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 0);
}

TEST_CASE(SmartPtrTest, TransferAllocator)
{
	Scope<TestObject> smartPtr = MakeScope<TestObject>(g_GlobalAllocators.pStack, 500);
	TEST_EQUAL(static_cast<i32>(*smartPtr.Get()), 500);

	u32 allocatedSize = ((MallocAllocator*)(g_GlobalAllocators.pMalloc))->GetAllocatedMemorySize();

	// Transfer the smart pointer to a different allocator
	Result result = smartPtr.Transfer(g_GlobalAllocators.pMalloc);
	TEST_SUCCESS(result);

	u32 allocatedSizeAfter = ((MallocAllocator*)(g_GlobalAllocators.pMalloc))->GetAllocatedMemorySize();
	TEST_ASSERT(allocatedSizeAfter ==
				allocatedSize + sizeof(TestObject)); // Ensure memory was allocated in the new allocator

	// After transfer, the smart pointer should still be valid
	TEST_NOT_NULL(smartPtr.Get());
	TEST_EQUAL(static_cast<i32>(*smartPtr.Get()), 500);

	// After transfer, the destructor should not have been called yet
	TEST_EQUAL(TestObject::s_ConstructorCount, 1);
	TEST_EQUAL(TestObject::s_DestructorCount, 0);
	TEST_EQUAL(TestObject::s_CopyConstructorCount, 0);
	TEST_EQUAL(TestObject::s_MoveConstructorCount, 0);
}