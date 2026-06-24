#pragma once

#define NTT_MAX_BACKTRACE_DEPTH 12
#include "common.h"

namespace ntt {

class Backtrace
{
	NTT_RAII(Backtrace)

public:
	Result Capture();
	Result Print() const;

private:
	void* m_Addresses[NTT_MAX_BACKTRACE_DEPTH];
	u32	  m_Depth;
};

} // namespace ntt
