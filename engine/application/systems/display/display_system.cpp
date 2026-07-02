#include "display_system.h"
#include "display_driver.h"
#include "drivers/ntt_glfw/ntt_glfw.h"

namespace ntt {

NTT_OBJECT_DERIVED_DEFINE(DisplaySystem, System)

DisplaySystem::DisplaySystem()
{
}

DisplaySystem::~DisplaySystem()
{
}

Result DisplaySystem::InitializeImpl()
{
	NTT_ASSERT_RESULT_SUCCESS(RegisterGLFWDisplayDriver());

	NTT_DISPLAY_INFO("Display system initialized successfully.");
	m_pWindowIDStorage = MakeScope<Storage<WindowInfo>>(g_GlobalAllocators.pMalloc);

	if (g_DisplayDriver.Initialize == nullptr || g_DisplayDriver.CreateWindow == nullptr ||
		g_DisplayDriver.DestroyWindow == nullptr || g_DisplayDriver.ShouldCloseWindow == nullptr ||
		g_DisplayDriver.OnBeginFrame == nullptr || g_DisplayDriver.OnEndFrame == nullptr)
	{
		NTT_DISPLAY_ERROR("Display driver is not initialized.");
		return RESULT_DISPLAY_DRIVER_NOT_INITIALIZED;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_DisplayDriver.Initialize());

	if (m_pWindowIDStorage == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to allocate memory for window ID storage.");
		return RESULT_OUT_OF_MEMORY;
	}

	return RESULT_SUCCESS;
}

Result DisplaySystem::ShutdownImpl()
{
	NTT_DISPLAY_INFO("Display system shut down successfully.");
	NTT_ASSERT_RESULT_SUCCESS(g_DisplayDriver.Shutdown());
	m_pWindowIDStorage.Reset();
	return RESULT_SUCCESS;
}

WindowID DisplaySystem::CreateWindow(u32 width, u32 height, const char* title)
{
	WindowInfo windowInfo;
	windowInfo.pWindowHandle =
		ALLOCATOR_SAFE(g_GlobalAllocators.pMalloc)->Allocate(g_DisplayDriver.GetWindowHandleSize());
	Result result = g_DisplayDriver.CreateWindow(width, height, title, windowInfo.pWindowHandle);

	if (result != RESULT_SUCCESS)
	{
		NTT_DISPLAY_ERROR("Failed to create window. Error code: %d", result);
		return INVALID_WINDOW_ID;
	}

	windowInfo.width  = width;
	windowInfo.height = height;

	return m_pWindowIDStorage->Add(windowInfo);
}

Result DisplaySystem::DestroyWindow(WindowID windowID)
{
	WindowInfo* pWindowInfo = m_pWindowIDStorage->Get(windowID);

	if (pWindowInfo == nullptr)
	{
		NTT_DISPLAY_ERROR("Invalid window ID: %u", windowID);
		return RESULT_INACTIVE_STORAGE_INDEX;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_DisplayDriver.DestroyWindow(pWindowInfo->pWindowHandle));
	NTT_ASSERT_RESULT_SUCCESS(pWindowInfo->pWindowHandle.Free());
	NTT_ASSERT_RESULT_SUCCESS(m_pWindowIDStorage->Remove(windowID));

	return RESULT_SUCCESS;
}

bool DisplaySystem::ShouldCloseWindow(WindowID windowID)
{
	WindowInfo* pWindowInfo = m_pWindowIDStorage->Get(windowID);

	if (pWindowInfo == nullptr)
	{
		NTT_DISPLAY_ERROR("Invalid window ID: %u", windowID);
		return true; // Return true to indicate the window should be closed
	}

	return g_DisplayDriver.ShouldCloseWindow(pWindowInfo->pWindowHandle);
}

Result DisplaySystem::OnBeginFrame(WindowID windowID)
{
	if (g_DisplayDriver.OnBeginFrame == nullptr)
	{
		NTT_DISPLAY_ERROR("Display driver OnBeginFrame function is not initialized.");
		return RESULT_DISPLAY_DRIVER_NOT_INITIALIZED;
	}

	return g_DisplayDriver.OnBeginFrame(m_pWindowIDStorage->Get(windowID)->pWindowHandle);
}

Result DisplaySystem::OnEndFrame(WindowID windowID)
{
	if (g_DisplayDriver.OnEndFrame == nullptr)
	{
		NTT_DISPLAY_ERROR("Display driver OnEndFrame function is not initialized.");
		return RESULT_DISPLAY_DRIVER_NOT_INITIALIZED;
	}

	return g_DisplayDriver.OnEndFrame(m_pWindowIDStorage->Get(windowID)->pWindowHandle);
}

Result DisplaySystem::SetOnWindowResizeCallback(WindowID windowID, OnWindowResizeCallback callback, void* pUserData)
{
	WindowInfo* pWindowInfo = m_pWindowIDStorage->Get(windowID);
	if (pWindowInfo == nullptr)
	{
		NTT_DISPLAY_ERROR("Invalid window ID: %u", windowID);
		return RESULT_INACTIVE_STORAGE_INDEX;
	}

	if (g_DisplayDriver.SetOnWindowResizeCallback == nullptr)
	{
		NTT_DISPLAY_ERROR("Display driver SetOnWindowResizeCallback function is not initialized.");
		return RESULT_DISPLAY_DRIVER_NOT_INITIALIZED;
	}

	return g_DisplayDriver.SetOnWindowResizeCallback(pWindowInfo->pWindowHandle, callback, pUserData);
}

} // namespace ntt