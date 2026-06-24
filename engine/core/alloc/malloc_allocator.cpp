#include "malloc_allocator.h"
#include "backtrace/backtrace.h"
#include <cstdlib>
#include <cstring>

namespace ntt {

struct MemoryBlockHeader
{
	u32 size;
#if NTT_LOG_MEMORY
	Backtrace				  backtrace;
	struct MemoryBlockHeader* next;
	struct MemoryBlockHeader* prev;
#endif // NTT_LOG_MEMORY
};

MemoryBlockHeader* g_HeadMemoryBlock = nullptr;

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

void* MallocAllocator::Allocate(u32 size)
{
	m_AllocatedMemorySize += size;

	void* pBlock = malloc(sizeof(MemoryBlockHeader) + size);

	if (pBlock == nullptr)
	{
		return nullptr;
	}

	MemoryBlockHeader* pHeader = static_cast<MemoryBlockHeader*>(pBlock);
	pHeader->size			   = size;

#if NTT_LOG_MEMORY
	pHeader->backtrace.Capture();
	pHeader->next = g_HeadMemoryBlock;
	pHeader->prev = nullptr;
	if (g_HeadMemoryBlock != nullptr)
	{
		g_HeadMemoryBlock->prev = pHeader;
	}
	g_HeadMemoryBlock = pHeader;
#endif // NTT_LOG_MEMORY

	return static_cast<void*>(pHeader + sizeof(MemoryBlockHeader));
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

#if NTT_LOG_MEMORY
	MemoryBlockHeader* next = pHeader->next;
	MemoryBlockHeader* prev = pHeader->prev;

	if (next != nullptr)
	{
		next->prev = prev;
	}

	if (prev != nullptr)
	{
		prev->next = next;
	}
#endif // NTT_LOG_MEMORY

	m_AllocatedMemorySize -= pHeader->size;
	free(pHeader);

	return result;
}

Result MallocAllocator::Shutdown()
{
	if (m_AllocatedMemorySize != 0)
	{
#if NTT_LOG_MEMORY
		setConsoleColor(CONSOLE_COLOR_RED);
		print("[ERROR] Memory leak detected. Allocated memory size: %u bytes\n", m_AllocatedMemorySize);
		setConsoleColor(CONSOLE_COLOR_CYAN);
		print("[LOG] Leaked memory blocks:\n");
		setConsoleColor(CONSOLE_COLOR_YELLOW);

		MemoryBlockHeader* current = g_HeadMemoryBlock;
		while (current != nullptr)
		{
			print("Leaked block of size: %u bytes\n", current->size);
			print("Allocation backtrace:\n");
			current->backtrace.Print();
			current = current->next;
		}

#endif // NTT_LOG_MEMORY

		return RESULT_MEMORY_LEAK;
	}

	return RESULT_SUCCESS;
}

} // namespace ntt