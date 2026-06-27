#include "stack_allocator.h"

namespace ntt {

StackAllocator::StackAllocator(u32 size, IAllocator* pBackingAllocator)
	: m_Size(size)
	, m_pBackingAllocator(pBackingAllocator)
	, m_pMemoryBlock(nullptr)
	, m_pCurrentPtr(nullptr)
{
}

StackAllocator::~StackAllocator()
{
}

Result StackAllocator::Initialize()
{
	if (m_pBackingAllocator == nullptr)
	{
		return RESULT_BACKING_ALLOCATOR_NOT_PROVIDED;
	}

	m_pMemoryBlock = m_pBackingAllocator->Allocate(m_Size);

	if (m_pMemoryBlock == nullptr)
	{
		return RESULT_OUT_OF_MEMORY;
	}

	m_pCurrentPtr = m_pMemoryBlock;

	return RESULT_SUCCESS;
}

Result StackAllocator::Shutdown()
{
	if (m_pBackingAllocator == nullptr)
	{
		return RESULT_BACKING_ALLOCATOR_NOT_PROVIDED;
	}

	if (m_pMemoryBlock == nullptr)
	{
		return RESULT_UNKNOWN;
	}

	NTT_ASSERT_RESULT_SUCCESS(m_pBackingAllocator->Free(m_pMemoryBlock, m_Size));

	return RESULT_SUCCESS;
}

void* StackAllocator::Allocate(u32 size)
{
	m_pCurrentPtr = static_cast<u8*>(m_pCurrentPtr) + size;
	return m_pCurrentPtr;
}

Result StackAllocator::Free(void* ptr, u32 size)
{
	NTT_UNUSED(ptr);
	NTT_UNUSED(size);
	return RESULT_SUCCESS;
}

} // namespace ntt
