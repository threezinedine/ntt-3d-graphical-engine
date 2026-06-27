#pragma once

#include "iallocator.h"
#include "malloc_allocator.h"
#include "stack_allocator.h"

namespace ntt {

struct GlobalAllocators
{
	IAllocator* pMalloc;
	IAllocator* pStack;

	static Result Register();
	static Result Unregister();

	static Result Initialize();
	static Result Shutdown();
};

extern GlobalAllocators g_GlobalAllocators;

#define ALLOCATOR_SAFE(pAllocator) (pAllocator == nullptr ? g_GlobalAllocators.pMalloc : pAllocator)

#define REALLOCATE(ptr) new ((ptr))

} // namespace ntt
