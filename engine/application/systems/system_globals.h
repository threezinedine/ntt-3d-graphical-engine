#pragma once

#include "services.h"

namespace ntt {

class DisplaySystem;

class SystemGlobals
{
public:
	static Scope<DisplaySystem> pDisplaySystem;

public:
	static Result Initialize();
	static Result Shutdown();
};

} // namespace ntt
