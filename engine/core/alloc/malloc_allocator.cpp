#include "malloc_allocator.h"
#include <cstdlib>
#include <cstring>

namespace ntt {

MallocAllocator::MallocAllocator()
{
}

MallocAllocator::~MallocAllocator()
{
}

Result MallocAllocator::Initialize()
{
	return RESULT_SUCCESS;
}

void* MallocAllocator::Allocate(u32 size)
{
	return malloc(size);
}

void MallocAllocator::Free(void* ptr)
{
	free(ptr);
}

Result MallocAllocator::Shutdown()
{
	return RESULT_SUCCESS;
}

} // namespace ntt