#include "system_globals.h"
#include "display/display_system.h"
#include "render/render.h"

namespace ntt {

Scope<DisplaySystem> SystemGlobals::pDisplaySystem;
Scope<RenderSystem>	 SystemGlobals::pRenderSystem;

Result SystemGlobals::Initialize()
{
	SystemGlobals::pDisplaySystem = MakeScope<DisplaySystem>(g_GlobalAllocators.pMalloc);

	if (SystemGlobals::pDisplaySystem == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to allocate memory for DisplaySystem.");
		return RESULT_OUT_OF_MEMORY;
	}

	SystemGlobals::pRenderSystem = MakeScope<RenderSystem>(g_GlobalAllocators.pMalloc);

	if (SystemGlobals::pRenderSystem == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to allocate memory for RenderSystem.");
		return RESULT_OUT_OF_MEMORY;
	}

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->Initialize());
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->Initialize());

	return RESULT_SUCCESS;
}

Result SystemGlobals::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->Shutdown());

	SystemGlobals::pRenderSystem.Reset();
	SystemGlobals::pDisplaySystem.Reset();
	return RESULT_SUCCESS;
}

} // namespace ntt
