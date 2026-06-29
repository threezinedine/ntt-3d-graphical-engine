#pragma once

#include "services.h"

namespace ntt {

struct RenderDriver
{
	Result (*Initialize)();
	Result (*Shutdown)();
	Result (*CreateRenderContext)(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle);
	Result (*DestroyRenderContext)(Pointer<void>& pRenderContextHandle);
	Result (*StartRender)(Pointer<void> pDriverHandle);
	Result (*EndRender)(Pointer<void> pDriverHandle);
	Result (*Present)(Pointer<void> pDriverHandle);
	u32 (*GetRenderContextHandleSize)();
};

extern RenderDriver g_RenderDriver;

} // namespace ntt
