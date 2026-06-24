#pragma once

#include "common.h"

namespace ntt {

template <typename T>
struct Optional
{
	Result result;
	T	   value;
};

} // namespace ntt
