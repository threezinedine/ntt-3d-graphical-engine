#include "alloc.h"

#if NTT_UNITTEST
#define NTT_STACK_ALLOCATOR_SIZE 1024 * 1024 * 10 // 10 MB
#else
#define NTT_STACK_ALLOCATOR_SIZE 1024 * 1024 * 1024 // 1 GB
#endif

namespace ntt {

GlobalAllocators g_GlobalAllocators;

Result GlobalAllocators::Register()
{
	g_GlobalAllocators.pMalloc = new MallocAllocator();
	g_GlobalAllocators.pStack  = new StackAllocator(NTT_STACK_ALLOCATOR_SIZE, g_GlobalAllocators.pMalloc);

	return RESULT_SUCCESS;
}

Result GlobalAllocators::Unregister()
{
	delete g_GlobalAllocators.pStack;
	g_GlobalAllocators.pStack = nullptr;

	delete g_GlobalAllocators.pMalloc;
	g_GlobalAllocators.pMalloc = nullptr;

	return RESULT_SUCCESS;
}

Result GlobalAllocators::Initialize()
{
	NTT_ASSERT_RESULT_SUCCESS(g_GlobalAllocators.pMalloc->Initialize());
	NTT_ASSERT_RESULT_SUCCESS(g_GlobalAllocators.pStack->Initialize());
	return RESULT_SUCCESS;
}

Result GlobalAllocators::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(g_GlobalAllocators.pStack->Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(g_GlobalAllocators.pMalloc->Shutdown());
	return RESULT_SUCCESS;
}

} // namespace ntt
