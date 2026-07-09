#pragma once

#include "services.h"

namespace ntt {

class DisplaySystem;
class RenderSystem;

class SystemGlobals
{
public:
	static Scope<DisplaySystem> pDisplaySystem;
	static Scope<RenderSystem>	pRenderSystem;

public:
	static Result Initialize();
	static Result Shutdown();
};

#define NTT_DISPLAY_SYSTEM (SystemGlobals::pDisplaySystem)
#define NTT_RENDER_SYSTEM  (SystemGlobals::pRenderSystem)

} // namespace ntt
