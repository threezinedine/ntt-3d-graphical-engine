#include "alloc.h"

namespace ntt {

GlobalAllocators g_GlobalAllocators;

Result GlobalAllocators::Register()
{
	g_GlobalAllocators.pMalloc = new MallocAllocator();

	return RESULT_SUCCESS;
}

Result GlobalAllocators::Unregister()
{
	delete g_GlobalAllocators.pMalloc;
	g_GlobalAllocators.pMalloc = nullptr;

	return RESULT_SUCCESS;
}

Result GlobalAllocators::Initialize()
{
	NTT_ASSERT_RESULT_SUCCESS(g_GlobalAllocators.pMalloc->Initialize());
	return RESULT_SUCCESS;
}

Result GlobalAllocators::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(g_GlobalAllocators.pMalloc->Shutdown());
	return RESULT_SUCCESS;
}

} // namespace ntt
