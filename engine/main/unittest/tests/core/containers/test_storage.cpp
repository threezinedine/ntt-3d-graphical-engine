#include "core.h"
#include "tests/core/container_test_object.h"
#include "utilities/utilities.h"

using namespace ntt;

class StorageTest : public TestSuite
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

TEST_SUITE(StorageTest)

TEST_CASE(StorageTest, ClearStorage)
{
	Storage<TestObject> storage;
	TEST_EQUAL(storage.GetCount(), 0);

	TEST_ASSERT(storage.Clear() == RESULT_SUCCESS);
}

TEST_CASE(StorageTest, AddAndGetElements)
{
	Storage<TestObject> storage;

	u32			index1 = storage.Add();
	TestObject* pObj1  = storage.Get(index1);
	pObj1->SetValue(10);

	u32			index2 = storage.Add();
	TestObject* pObj2  = storage.Get(index2);
	pObj2->SetValue(20);

	u32			index3 = storage.Add();
	TestObject* pObj3  = storage.Get(index3);
	pObj3->SetValue(30);

	TEST_EQUAL(storage.GetCount(), 3);
	TEST_EQUAL((i32)*storage.Get(index1), 10);
	TEST_EQUAL((i32)*storage.Get(index2), 20);
	TEST_EQUAL((i32)*storage.Get(index3), 30);
}

TEST_CASE(StorageTest, AddAndClearElements)
{
	Storage<TestObject> storage;

	{
		u32			index = storage.Add();
		TestObject* pObj  = storage.Get(index);
		pObj->SetValue(10);
	}
	{
		u32			index = storage.Add();
		TestObject* pObj  = storage.Get(index);
		pObj->SetValue(20);
	}
	{
		u32			index = storage.Add();
		TestObject* pObj  = storage.Get(index);
		pObj->SetValue(30);
	}

	TEST_EQUAL(storage.GetCount(), 3);

	TEST_ASSERT(storage.Clear() == RESULT_SUCCESS);
	TEST_EQUAL(storage.GetCount(), 0);
}

TEST_CASE(StorageTest, AddAndRemoveElements)
{
	Storage<TestObject> storage;

	u32 index1 = storage.Add();
	storage.Get(index1)->SetValue(10);

	u32 index2 = storage.Add();
	storage.Get(index2)->SetValue(20);

	u32 index3 = storage.Add();
	storage.Get(index3)->SetValue(30);

	TEST_EQUAL(storage.GetCount(), 3);

	TEST_ASSERT(storage.Remove(index2) == RESULT_SUCCESS);

	TEST_EQUAL(storage.GetCount(), 2);
	TEST_EQUAL((i32)*storage.Get(index1), 10);
	TEST_EQUAL((i32)*storage.Get(index3), 30);

	u32 newIndex = storage.Add();
	storage.Get(newIndex)->SetValue(40);
	TEST_EQUAL(storage.GetCount(), 3);
	TEST_EQUAL(index2, newIndex); // The new index should be the same as the removed index
}

TEST_CASE(StorageTest, RemoveInactiveElement)
{
	Storage<TestObject> storage;

	u32 index1 = storage.Add();
	storage.Get(index1)->SetValue(10);

	u32 index2 = storage.Add();
	storage.Get(index2)->SetValue(20);

	TEST_ASSERT(storage.Remove(index2) == RESULT_SUCCESS);
	TEST_ASSERT(storage.Remove(index2) == RESULT_INACTIVE_STORAGE_INDEX); // Attempting to remove an inactive element
	TEST_ASSERT(storage.Remove(5) == RESULT_INDEX_OUT_OF_BOUNDS);		  // Attempting to remove an out-of-bounds index
}

TEST_CASE(StorageTest, WorkWithTestObject)
{
	Storage<TestObject> storage;

	u32 index1 = storage.Add();
	storage.Get(index1)->SetValue(10);

	u32 index2 = storage.Add();
	storage.Get(index2)->SetValue(20);

	TEST_EQUAL(TestObject::s_ConstructorCount, 0);
	TEST_EQUAL(TestObject::s_DestructorCount, 0);

	TEST_EQUAL((i32)*storage.Get(index1), 10);
	TEST_EQUAL((i32)*storage.Get(index2), 20);

	storage.Remove(index1);

	TEST_EQUAL(TestObject::s_DestructorCount, 1); // Destructor should be called for the removed element

	TEST_ASSERT(storage.Remove(index1) == RESULT_INACTIVE_STORAGE_INDEX); // Attempting to remove an inactive element
	TEST_ASSERT(storage.Remove(5) == RESULT_INDEX_OUT_OF_BOUNDS);		  // Attempting to remove an out-of-bounds index

	TEST_EQUAL(storage.GetCount(), 1);
	TEST_EQUAL(storage.Get(index1), nullptr); // Accessing inactive element should return nullptr
}