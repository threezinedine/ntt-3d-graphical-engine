#pragma once

#include "display_system.h"
#include "services.h"

namespace ntt {

struct DisplayDriver
{
	Result (*Initialize)();
	Result (*Shutdown)();
	Result (*CreateWindow)(u32 width, u32 height, const char* title, Pointer<void>& pWindowHandle, bool shareContext);
	bool (*ShouldCloseWindow)(Pointer<void> pWindowHandle);
	Result (*DestroyWindow)(Pointer<void> pWindowHandle);
	Result (*OnBeginFrame)(Pointer<void> pDriverHandle);
	Result (*OnEndFrame)(Pointer<void> pDriverHandle);
	u32 (*GetWindowHandleSize)();
	void* (*GetWindowHandle)(Pointer<void> pWindowHandle);
	Vec2u (*GetWindowSize)(Pointer<void> pWindowHandle);
	Result (*SetOnWindowResizeCallback)(Pointer<void> pWindowHandle, OnWindowResizeCallback callback, void* pUserData);
};

extern DisplayDriver g_DisplayDriver;

} // namespace ntt