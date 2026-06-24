#pragma once

#include "iallocator.h"

namespace ntt {

class MallocAllocator : public IAllocator
{
public:
	MallocAllocator();
	virtual ~MallocAllocator() override;

	virtual Result			Initialize() override;
	virtual Optional<void*> Allocate(u32 size) override;
	virtual Result			Free(void* ptr, u32 size) override;
	virtual Result			Shutdown() override;

private:
	u32 m_AllocatedMemorySize = 0;
};

} // namespace ntt