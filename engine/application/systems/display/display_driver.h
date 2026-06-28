#pragma once

#include "services.h"

namespace ntt {

struct DisplayDriver
{
	void* pDriverHandle;

	Result (*Initialize)();
	Result (*Shutdown)();
	Result (*CreateWindow)(u32 width, u32 height, const char* title, void** pWindowHandle);
	bool (*ShouldCloseWindow)(void* pWindowHandle);
	Result (*DestroyWindow)(void* pWindowHandle);
	Result (*OnBeginFrame)(void* pDriverHandle);
	Result (*OnEndFrame)(void* pDriverHandle);
};

extern DisplayDriver g_DisplayDriver;

} // namespace ntt