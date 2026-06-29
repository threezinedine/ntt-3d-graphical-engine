#pragma once

#include "services.h"

namespace ntt {

struct RenderDriver
{
	Result (*Initialize)();
	Result (*Shutdown)();
	Result (*StartRender)(Pointer<void> pDriverHandle);
	Result (*EndRender)(Pointer<void> pDriverHandle);
	Result (*Present)(Pointer<void> pDriverHandle);
	u32 (*GetRenderDriverHandleSize)();
};

extern RenderDriver g_RenderDriver;

} // namespace ntt
