#pragma once

#include "systems/system.h"

namespace ntt {

class RenderSystem;

typedef u32		   WindowID;
constexpr WindowID INVALID_WINDOW_ID = static_cast<u32>(-1);

typedef Result (*OnWindowResizeCallback)(u32 width, u32 height, void* pUserData);

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
	bool	 ShouldCloseWindow(WindowID windowID) const;
	bool	 IsWindowActive(WindowID windowID) const;

	Result SetOnWindowResizeCallback(WindowID windowID, OnWindowResizeCallback callback, void* pUserData);

public:
	Result OnBeginFrame(WindowID windowID);
	Result OnEndFrame(WindowID windowID);

public:
	bool ShouldApplicationClose() const;

public:
	struct WindowInfo
	{
		Pointer<void> pWindowHandle;
		u32			  width;
		u32			  height;
	};

	friend class RenderSystem;

private:
	Scope<Storage<WindowInfo>> m_pWindowIDStorage;
};

} // namespace ntt
