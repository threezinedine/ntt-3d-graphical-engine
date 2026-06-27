#pragma once

#include "common.h"
#include "optional.h"

namespace ntt {

class IAllocator;

class IAllocator
{
public:
	virtual ~IAllocator() = default;

	virtual Result Initialize()				 = 0;
	virtual void*  Allocate(u32 size)		 = 0;
	virtual Result Free(void* ptr, u32 size) = 0;
	virtual Result Shutdown()				 = 0;
};

} // namespace ntt