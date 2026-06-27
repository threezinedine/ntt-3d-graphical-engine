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
	virtual Result Free(void* ptr, u32 size) override;
	virtual Result Shutdown() override;

#if NTT_UNITTEST
	inline u32 GetAllocatedMemorySize() const
	{
		return m_AllocatedMemorySize;
	}
#endif // NTT_UNITTEST

private:
	u32 m_AllocatedMemorySize = 0;
};

} // namespace ntt