#pragma once

#include "iallocator.h"

namespace ntt {

class MallocAllocator : public IAllocator
{
public:
	MallocAllocator();
	virtual ~MallocAllocator() override;

	virtual Result Initialize() override;
	virtual void*  Allocate(u32 size) override;
	virtual void   Free(void* ptr) override;
	virtual Result Shutdown() override;
};

} // namespace ntt