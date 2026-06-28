#include "core.h"
#include "tests/core/container_test_object.h"
#include "utilities/utilities.h"

using namespace ntt;

class MapTest : public TestSuite
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

TEST_SUITE(MapTest)

TEST_CASE(MapTest, InsertAndAccessElements)
{
	Map<i32, i32> map(16, [](const i32& key) { return static_cast<u32>(key); });

	TEST_SUCCESS(map.Insert(1, 100));
	TEST_SUCCESS(map.Insert(2, 200));
	TEST_SUCCESS(map.Insert(3, 300));

	TEST_EQUAL(map[1], 100);
	TEST_EQUAL(map[2], 200);
	TEST_EQUAL(map[3], 300);
}

TEST_CASE(MapTest, InsertDuplicateKey)
{
	Map<i32, i32> map(16, [](const i32& key) { return static_cast<u32>(key); });

	TEST_SUCCESS(map.Insert(1, 100));
	TEST_SUCCESS(map.Insert(1, 200)); // Insert duplicate key

	TEST_EQUAL(map[1], 200); // The value should be updated to the new value
}

TEST_CASE(MapTest, TestObject)
{
	Map<i32, TestObject> map(16, [](const i32& key) { return static_cast<u32>(key); });

	TEST_SUCCESS(map.Insert(1, TestObject(100)));
	TEST_SUCCESS(map.Insert(2, TestObject(200)));

	TEST_EQUAL(static_cast<i32>(map[1]), 100);
	TEST_EQUAL(static_cast<i32>(map[2]), 200);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 2);
	TEST_EQUAL(TestObject::s_DestructorCount, 2); // Destructors should not have been called yet

	// Clear the map to trigger destructors
	TEST_SUCCESS(map.Clear());

	// After clearing the map, destructors should have been called
	TEST_EQUAL(TestObject::s_DestructorCount, 4);
}

TEST_CASE(MapTest, Remap)
{
	Map<i32, i32> map(4, [](const i32& key) { return static_cast<u32>(key); });

	TEST_SUCCESS(map.Insert(1, 100));
	TEST_SUCCESS(map.Insert(2, 200));
	TEST_SUCCESS(map.Insert(3, 300));

	// Remap to a larger bucket count
	TEST_SUCCESS(map.Remap(33));

	// Verify that the elements are still accessible after remapping
	TEST_EQUAL(map[1], 100);
	TEST_EQUAL(map[2], 200);
	TEST_EQUAL(map[3], 300);
}

TEST_CASE(MapTest, RemapTestObject)
{
	Map<i32, TestObject> map(4, [](const i32& key) { return static_cast<u32>(key); });

	TEST_SUCCESS(map.Insert(1, TestObject(100)));
	TEST_SUCCESS(map.Insert(2, TestObject(200)));

	// Remap to a larger bucket count
	TEST_SUCCESS(map.Remap(33));

	// Verify that the elements are still accessible after remapping
	TEST_EQUAL(static_cast<i32>(map[1]), 100);
	TEST_EQUAL(static_cast<i32>(map[2]), 200);

	// Check constructor and destructor counts
	TEST_EQUAL(TestObject::s_ConstructorCount, 2);
	TEST_EQUAL(TestObject::s_DestructorCount, 2); // Destructors should not have been called yet

	// Clear the map to trigger destructors
	TEST_SUCCESS(map.Clear());

	// After clearing the map, destructors should have been called
	TEST_EQUAL(TestObject::s_DestructorCount, 4);
}

TEST_CASE(MapTest, ForLoop)
{
	Map<i32, i32> map(16, [](const i32& key) { return static_cast<u32>(key); });

	TEST_SUCCESS(map.Insert(1, 100));
	TEST_SUCCESS(map.Insert(2, 200));
	TEST_SUCCESS(map.Insert(3, 300));

	i32 sum = 0;
	for (auto it = map.begin(); it != map.end(); ++it)
	{
		sum += it.value();
	}

	TEST_EQUAL(sum, 600); // 100 + 200 + 300
}