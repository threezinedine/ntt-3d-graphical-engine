#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class MallocTest : public TestSuite
{
	void OnBeforeEachTestImpl() override
	{
		GlobalAllocators::Register();
		GlobalAllocators::Initialize();
	}

	void OnAfterEachTestImpl() override
	{
		GlobalAllocators::Shutdown();
		GlobalAllocators::Unregister();
	}
};

TEST_SUITE(MallocTest)

TEST_CASE_ISOLATED(MallocTest, Setup)
{
	TEST_SUCCESS(GlobalAllocators::Register());
	TEST_SUCCESS(GlobalAllocators::Initialize());
	TEST_SUCCESS(GlobalAllocators::Shutdown());
	TEST_SUCCESS(GlobalAllocators::Unregister());
}

TEST_CASE_ISOLATED(MallocTest, AllocateAndFreeMemory)
{
	TEST_SUCCESS(GlobalAllocators::Register());
	TEST_SUCCESS(GlobalAllocators::Initialize());

	void* ptr = g_GlobalAllocators.pMalloc->Allocate(1024);
	TEST_NOT_NULL(ptr);

	TEST_SUCCESS(g_GlobalAllocators.pMalloc->Free(ptr, 1024));

	TEST_SUCCESS(GlobalAllocators::Shutdown());
	TEST_SUCCESS(GlobalAllocators::Unregister());
}

TEST_CASE_ISOLATED(MallocTest, NotFreeingMemory)
{
	TEST_SUCCESS(GlobalAllocators::Register());
	TEST_SUCCESS(GlobalAllocators::Initialize());

	void* ptr = g_GlobalAllocators.pMalloc->Allocate(1024);
	TEST_NOT_NULL(ptr);

	TEST_EQUAL(GlobalAllocators::Shutdown(), RESULT_MEMORY_LEAK);
	TEST_SUCCESS(GlobalAllocators::Unregister());
}

TEST_CASE_ISOLATED(MallocTest, UnmatchedFreeSize)
{
	TEST_SUCCESS(GlobalAllocators::Register());
	TEST_SUCCESS(GlobalAllocators::Initialize());

	void* ptr = g_GlobalAllocators.pMalloc->Allocate(1024);
	TEST_NOT_NULL(ptr);

	TEST_EQUAL(g_GlobalAllocators.pMalloc->Free(ptr, 1000), RESULT_UNMATCHED_FREE_SIZE);

	TEST_SUCCESS(GlobalAllocators::Shutdown());
	TEST_SUCCESS(GlobalAllocators::Unregister());
}