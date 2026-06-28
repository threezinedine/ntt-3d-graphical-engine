#pragma once

#include "systems/system.h"

namespace ntt {

typedef u32		   WindowID;
constexpr WindowID INVALID_WINDOW_ID = static_cast<u32>(-1);

class DisplaySystem : public System
{
	NTT_OBJECT_DERIVED_DECLARE(DisplaySystem, System)

public:
	DisplaySystem();
	virtual ~DisplaySystem();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

public:
	WindowID CreateWindow(u32 width, u32 height, const char* title);
	Result	 DestroyWindow(WindowID windowID);
	bool	 ShouldCloseWindow(WindowID windowID);

public:
	Result OnBeginFrame(WindowID windowID);
	Result OnEndFrame(WindowID windowID);

public:
	struct WindowInfo
	{
		void* pWindowHandle;
	};

private:
	Scope<Storage<WindowInfo>> m_pWindowIDStorage;
};

} // namespace ntt
