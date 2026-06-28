#include "ntt_glfw_driver.h"
#include "core.h"
#include <GLFW/glfw3.h>

namespace ntt {

static Result GLFWDisplayDriver_Initialize();
static Result GLFWDisplayDriver_Shutdown();
static Result GLFWDisplayDriver_CreateWindow(u32 width, u32 height, const char* title, void** pWindowHandle);
static bool	  GLFWDisplayDriver_ShouldCloseWindow(void* pWindowHandle);
static Result GLFWDisplayDriver_DestroyWindow(void* pWindowHandle);
static Result GLFWDisplayDriver_OnBeginFrame(void* pDriverHandle);
static Result GLFWDisplayDriver_OnEndFrame(void* pDriverHandle);

Result RegisterGLFWDisplayDriver()
{
	g_DisplayDriver.pDriverHandle	  = nullptr;
	g_DisplayDriver.Initialize		  = GLFWDisplayDriver_Initialize;
	g_DisplayDriver.Shutdown		  = GLFWDisplayDriver_Shutdown;
	g_DisplayDriver.CreateWindow	  = GLFWDisplayDriver_CreateWindow;
	g_DisplayDriver.ShouldCloseWindow = GLFWDisplayDriver_ShouldCloseWindow;
	g_DisplayDriver.DestroyWindow	  = GLFWDisplayDriver_DestroyWindow;
	g_DisplayDriver.OnBeginFrame	  = GLFWDisplayDriver_OnBeginFrame;
	g_DisplayDriver.OnEndFrame		  = GLFWDisplayDriver_OnEndFrame;
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_Initialize()
{
	if (!glfwInit())
	{
		NTT_DISPLAY_ERROR("Failed to initialize GLFW.");
		return RESULT_GLFW_INIT_FAILED;
	}

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_Shutdown()
{
	glfwTerminate();
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_CreateWindow(u32 width, u32 height, const char* title, void** pWindowHandle)
{
	if (pWindowHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GLFWwindow* pWindow = glfwCreateWindow(static_cast<i32>(width), static_cast<i32>(height), title, nullptr, nullptr);
	if (pWindow == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to create GLFW window.");
		return RESULT_GLFW_WINDOW_CREATION_FAILED;
	}

	*pWindowHandle = pWindow;

	glfwMakeContextCurrent(pWindow);

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_DestroyWindow(void* pWindowHandle)
{
	if (pWindowHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GLFWwindow* pWindow = static_cast<GLFWwindow*>(pWindowHandle);
	glfwDestroyWindow(pWindow);

	return RESULT_SUCCESS;
}

static bool GLFWDisplayDriver_ShouldCloseWindow(void* pWindowHandle)
{
	if (pWindowHandle == nullptr)
	{
		return true; // If the window handle is null, consider it should close
	}

	GLFWwindow* pWindow = static_cast<GLFWwindow*>(pWindowHandle);
	return glfwWindowShouldClose(pWindow);
}

static Result GLFWDisplayDriver_OnBeginFrame(void* pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	glfwPollEvents();
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_OnEndFrame(void* pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	// For GLFW, we don't have a specific driver handle to use here.
	// This function can be used for any per-frame cleanup if needed.
	return RESULT_SUCCESS;
}

} // namespace ntt
