#pragma once

#include "common.h"
#include "iallocator.h"

namespace ntt {

class StackAllocator : public IAllocator
{
public:
	StackAllocator(u32 size, IAllocator* pBackingAllocator = nullptr);
	~StackAllocator();

	Result		  Initialize() override;
	Pointer<void> Allocate(u32 size) override;
	Result		  Free(void* ptr, u32 size) override;
	Result		  Shutdown() override;

private:
	u32			  m_Size;
	IAllocator*	  m_pBackingAllocator;
	Pointer<void> m_pMemoryBlock;
	void*		  m_pCurrentPtr;
};

} // namespace ntt