#pragma once

#include "common.h"

namespace ntt {

class IAllocator
{
public:
	virtual ~IAllocator();

	virtual void* Allocate(u32 size) = 0;
	virtual void  Free(void* ptr)	 = 0;
};

} // namespace ntt