#pragma once

#include "iallocator.h"
#include "malloc_allocator.h"

namespace ntt {

struct GlobalAllocators
{
	IAllocator* pMalloc;

	static Result Register();
	static Result Unregister();

	static Result Initialize();
	static Result Shutdown();
};

extern GlobalAllocators g_GlobalAllocators;

} // namespace ntt
