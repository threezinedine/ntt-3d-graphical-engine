#include "system_globals.h"
#include "display/display_system.h"

namespace ntt {

Scope<DisplaySystem> SystemGlobals::pDisplaySystem;

Result SystemGlobals::Initialize()
{
	SystemGlobals::pDisplaySystem = MakeScope<DisplaySystem>(g_GlobalAllocators.pMalloc);

	if (SystemGlobals::pDisplaySystem == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to allocate memory for DisplaySystem.");
		return RESULT_OUT_OF_MEMORY;
	}

	return RESULT_SUCCESS;
}

Result SystemGlobals::Shutdown()
{
	SystemGlobals::pDisplaySystem.Reset();
	return RESULT_SUCCESS;
}

} // namespace ntt
