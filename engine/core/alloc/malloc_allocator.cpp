#include "malloc_allocator.h"
#include "backtrace/backtrace.h"
#include <cstdlib>
#include <cstring>

namespace ntt {

struct MemoryBlockHeader
{
	u32 size;
#if NTT_LOG_MEMORY
	Backtrace backtrace;
#endif // NTT_LOG_MEMORY
};

MallocAllocator::MallocAllocator()
	: m_AllocatedMemorySize(0)
{
}

MallocAllocator::~MallocAllocator()
{
}

Result MallocAllocator::Initialize()
{
	return RESULT_SUCCESS;
}

Optional<void*> MallocAllocator::Allocate(u32 size)
{
	m_AllocatedMemorySize += size;
	Optional<void*> result;

	void* pBlock = malloc(sizeof(MemoryBlockHeader) + size);

	if (pBlock == nullptr)
	{
		result.result = RESULT_OUT_OF_MEMORY;
		return result;
	}

	MemoryBlockHeader* pHeader = static_cast<MemoryBlockHeader*>(pBlock);
	pHeader->size			   = size;

#if NTT_LOG_MEMORY
	pHeader->backtrace.Capture();
#endif // NTT_LOG_MEMORY

	result.value  = static_cast<void*>(pHeader + sizeof(MemoryBlockHeader));
	result.result = RESULT_SUCCESS;
	return result;
}

Result MallocAllocator::Free(void* ptr, u32 size)
{
	Result			   result  = RESULT_SUCCESS;
	MemoryBlockHeader* pHeader = static_cast<MemoryBlockHeader*>(ptr) - sizeof(MemoryBlockHeader);

	if (pHeader->size != size)
	{
		result = RESULT_UNMATCHED_FREE_SIZE;
#if NTT_LOG_MEMORY
		setConsoleColor(CONSOLE_COLOR_YELLOW);
		print("[WARNING] Unmatched free size. Allocated size: %u, Free size: %u\n", pHeader->size, size);
		setConsoleColor(CONSOLE_COLOR_CYAN);
		print("[LOG] Allocation backtrace:\n");
		setConsoleColor(CONSOLE_COLOR_YELLOW);
		pHeader->backtrace.Print();
		setConsoleColor(CONSOLE_COLOR_CYAN);
		print("[LOG] Free backtrace:\n");
		setConsoleColor(CONSOLE_COLOR_YELLOW);
		Backtrace freeBacktrace;
		freeBacktrace.Capture();
		freeBacktrace.Print();
#endif // NTT_LOG_MEMORY
	}

	m_AllocatedMemorySize -= pHeader->size;
	free(pHeader);

	return result;
}

Result MallocAllocator::Shutdown()
{
	if (m_AllocatedMemorySize != 0)
	{
		return RESULT_MEMORY_LEAK;
	}

	return RESULT_SUCCESS;
}

} // namespace ntt