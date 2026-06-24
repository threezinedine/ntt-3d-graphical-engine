#pragma once

#include "common.h"

namespace ntt {

struct Globals
{
	i32	   argc;
	char** argv;
};

extern Globals g_Globals;

} // namespace ntt