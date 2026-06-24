#include "core.h"
#include "utilities/utilities.h"

class MallocTest : public ::ntt::TestSuite
{
	void OnBeforeEachTestImpl() override
	{
		ntt::GlobalAllocators::Register();
		ntt::GlobalAllocators::Initialize();
	}

	void OnAfterEachTestImpl() override
	{
		ntt::GlobalAllocators::Shutdown();
		ntt::GlobalAllocators::Unregister();
	}
};

TEST_SUITE(MallocTest)

TEST_CASE_ISOLATED(MallocTest, Setup)
{
	TEST_SUCCESS(ntt::GlobalAllocators::Register());
	TEST_SUCCESS(ntt::GlobalAllocators::Initialize());
	TEST_SUCCESS(ntt::GlobalAllocators::Shutdown());
	TEST_SUCCESS(ntt::GlobalAllocators::Unregister());
}

TEST_CASE_ISOLATED(MallocTest, AllocateAndFreeMemory)
{
	TEST_SUCCESS(ntt::GlobalAllocators::Register());
	TEST_SUCCESS(ntt::GlobalAllocators::Initialize());

	ntt::Optional<void*> result = ntt::g_GlobalAllocators.pMalloc->Allocate(1024);
	TEST_EQUAL(result.result, ntt::RESULT_SUCCESS);

	TEST_SUCCESS(ntt::g_GlobalAllocators.pMalloc->Free(result.value, 1024));

	TEST_SUCCESS(ntt::GlobalAllocators::Shutdown());
	TEST_SUCCESS(ntt::GlobalAllocators::Unregister());
}

TEST_CASE_ISOLATED(MallocTest, NotFreeingMemory)
{
	TEST_SUCCESS(ntt::GlobalAllocators::Register());
	TEST_SUCCESS(ntt::GlobalAllocators::Initialize());

	ntt::Optional<void*> pMemory = ntt::g_GlobalAllocators.pMalloc->Allocate(1024);
	TEST_EQUAL(pMemory.result, ntt::RESULT_SUCCESS);

	TEST_EQUAL(ntt::GlobalAllocators::Shutdown(), ntt::RESULT_MEMORY_LEAK);
	TEST_SUCCESS(ntt::GlobalAllocators::Unregister());
}

TEST_CASE_ISOLATED(MallocTest, UnmatchedFreeSize)
{
	TEST_SUCCESS(ntt::GlobalAllocators::Register());
	TEST_SUCCESS(ntt::GlobalAllocators::Initialize());

	ntt::Optional<void*> pMemoryResult = ntt::g_GlobalAllocators.pMalloc->Allocate(1024);
	TEST_EQUAL(pMemoryResult.result, ntt::RESULT_SUCCESS);

	TEST_EQUAL(ntt::g_GlobalAllocators.pMalloc->Free(pMemoryResult.value, 1000), ntt::RESULT_UNMATCHED_FREE_SIZE);

	TEST_SUCCESS(ntt::GlobalAllocators::Shutdown());
	TEST_SUCCESS(ntt::GlobalAllocators::Unregister());
}