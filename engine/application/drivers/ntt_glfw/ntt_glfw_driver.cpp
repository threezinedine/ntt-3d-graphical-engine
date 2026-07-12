#include "ntt_glfw_driver.h"
#include "core.h"

#include <GLFW/glfw3.h>

namespace ntt {

GLFWwindow* g_pDefaultWindow	  = nullptr;
static bool s_FirstWindowRequired = false;

struct GLFWDriverHandle
{
	GLFWwindow*			   pWindow;
	u32					   width;
	u32					   height;
	void*				   pUserData;
	OnWindowResizeCallback onWindowResizeCallback;
};

#define _CAST_DRIVER_HANDLE(handle, ret)                                                                               \
	reinterpret_cast<GLFWDriverHandle*>(handle.Get());                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return (ret);                                                                                              \
		}                                                                                                              \
	} while (0)

#define CAST_DRIVER_HANDLE(handle) _CAST_DRIVER_HANDLE(handle, RESULT_NULL_POINTER)

static Result GLFWDisplayDriver_Initialize();
static Result GLFWDisplayDriver_Shutdown();
static Result GLFWDisplayDriver_CreateWindow(
	u32 width, u32 height, const char* title, Pointer<void>& pWindowHandle, bool shareContext);
static bool	  GLFWDisplayDriver_ShouldCloseWindow(Pointer<void> pWindowHandle);
static Result GLFWDisplayDriver_DestroyWindow(Pointer<void> pWindowHandle);
static Result GLFWDisplayDriver_OnBeginFrame(Pointer<void> pDriverHandle);
static Result GLFWDisplayDriver_OnEndFrame(Pointer<void> pDriverHandle);
static u32	  GLFWDisplayDriver_GetWindowHandleSize();
static void*  GLFWDisplayDriver_GetWindowHandle(Pointer<void> pWindowHandle);
static Vec2u  GLFWDisplayDriver_GetWindowSize(Pointer<void> pWindowHandle);
static Result GLFWDisplayDriver_SetOnWindowResizeCallback(Pointer<void>			 pWindowHandle,
														  OnWindowResizeCallback callback,
														  void*					 pUserData);

Result RegisterGLFWDisplayDriver()
{
	g_DisplayDriver.Initialize				  = GLFWDisplayDriver_Initialize;
	g_DisplayDriver.Shutdown				  = GLFWDisplayDriver_Shutdown;
	g_DisplayDriver.CreateWindow			  = GLFWDisplayDriver_CreateWindow;
	g_DisplayDriver.ShouldCloseWindow		  = GLFWDisplayDriver_ShouldCloseWindow;
	g_DisplayDriver.DestroyWindow			  = GLFWDisplayDriver_DestroyWindow;
	g_DisplayDriver.OnBeginFrame			  = GLFWDisplayDriver_OnBeginFrame;
	g_DisplayDriver.OnEndFrame				  = GLFWDisplayDriver_OnEndFrame;
	g_DisplayDriver.GetWindowHandleSize		  = GLFWDisplayDriver_GetWindowHandleSize;
	g_DisplayDriver.GetWindowHandle			  = GLFWDisplayDriver_GetWindowHandle;
	g_DisplayDriver.GetWindowSize			  = GLFWDisplayDriver_GetWindowSize;
	g_DisplayDriver.SetOnWindowResizeCallback = GLFWDisplayDriver_SetOnWindowResizeCallback;
	return RESULT_SUCCESS;
}

static void GLFWDisplayDriver_OnWindowResize(GLFWwindow* pWindow, i32 width, i32 height);

static Result GLFWDisplayDriver_Initialize()
{
	s_FirstWindowRequired = false;

	if (!glfwInit())
	{
		NTT_DISPLAY_ERROR("Failed to initialize GLFW.");
		return RESULT_GLFW_INIT_FAILED;
	}

#if !NTT_PLATFORM_WEB
	if (NTT_ARG_BOOL(USE_VULKAN))
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
	else
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
#else  // NTT_PLATFORM_WEB
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif // !NTT_PLATFORM_WEB

	NTT_DISPLAY_INFO("GLFW loaded with version: %s", glfwGetVersionString());

	GLFWwindow* pWindow = glfwCreateWindow(1, 1, "NTT GLFW Driver", nullptr, nullptr);
	if (pWindow == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to create GLFW window for driver initialization.");
		glfwTerminate();
		return RESULT_GLFW_WINDOW_CREATION_FAILED;
	}

	glfwMakeContextCurrent(pWindow);
	glfwSetWindowSizeCallback(pWindow, GLFWDisplayDriver_OnWindowResize);

	g_pDefaultWindow = pWindow;

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_Shutdown()
{
	glfwTerminate();
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_CreateWindow(
	u32 width, u32 height, const char* title, Pointer<void>& pWindowHandle, bool shareContext)
{
	GLFWDriverHandle* pHandle = CAST_DRIVER_HANDLE(pWindowHandle);

	if (!s_FirstWindowRequired)
	{
		s_FirstWindowRequired = true;
		pHandle->pWindow	  = g_pDefaultWindow;
		pHandle->width		  = width;
		pHandle->height		  = height;
		glfwSetWindowUserPointer(pHandle->pWindow, pHandle);
		return RESULT_SUCCESS;
	}

	if (pWindowHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GLFWwindow* pWindow = glfwCreateWindow(
		static_cast<i32>(width), static_cast<i32>(height), title, nullptr, shareContext ? g_pDefaultWindow : nullptr);
	if (pWindow == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to create GLFW window.");
		return RESULT_GLFW_WINDOW_CREATION_FAILED;
	}

	pHandle->pWindow = pWindow;
	pHandle->width	 = width;
	pHandle->height	 = height;

	glfwSetWindowUserPointer(pWindow, pHandle);
	glfwSetWindowSizeCallback(pWindow, GLFWDisplayDriver_OnWindowResize);

	glfwMakeContextCurrent(pWindow);

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_DestroyWindow(Pointer<void> pWindowHandle)
{
	Pointer<GLFWDriverHandle> pHandle = pWindowHandle.Cast<GLFWDriverHandle>();

	if (pHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GLFWwindow* pWindow = static_cast<GLFWwindow*>(pHandle->pWindow);
	glfwDestroyWindow(pWindow);

	return RESULT_SUCCESS;
}

static bool GLFWDisplayDriver_ShouldCloseWindow(Pointer<void> pWindowHandle)
{
	Pointer<GLFWDriverHandle> pHandle = pWindowHandle.Cast<GLFWDriverHandle>();

	if (pHandle == nullptr)
	{
		return true; // If the window handle is null, consider it should close
	}

	GLFWwindow* pWindow = static_cast<GLFWwindow*>(pHandle->pWindow);
	return glfwWindowShouldClose(pWindow);
}

static Result GLFWDisplayDriver_OnBeginFrame(Pointer<void> pDriverHandle)
{
	GLFWDriverHandle* pHandle = CAST_DRIVER_HANDLE(pDriverHandle);
	glfwMakeContextCurrent(pHandle->pWindow);
	glfwPollEvents();

#if !NTT_PLATFORM_WEB
	if (glfwGetKey(pHandle->pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(pHandle->pWindow, true);
	}
#endif // !NTT_PLATFORM_WEB

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_OnEndFrame(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	return RESULT_SUCCESS;
}

static u32 GLFWDisplayDriver_GetWindowHandleSize()
{
	return (u32)sizeof(GLFWDriverHandle);
}

static void* GLFWDisplayDriver_GetWindowHandle(Pointer<void> pWindowHandle)
{
	Pointer<GLFWDriverHandle> pHandle = pWindowHandle.Cast<GLFWDriverHandle>();
	if (pHandle == nullptr || pHandle.Get() == nullptr)
	{
		return nullptr;
	}

	return pHandle->pWindow;
}

static Vec2u GLFWDisplayDriver_GetWindowSize(Pointer<void> pWindowHandle)
{
	Vec2u			  defaultSize{0, 0};
	GLFWDriverHandle* pHandle = _CAST_DRIVER_HANDLE(pWindowHandle, defaultSize);

	return Vec2u{pHandle->width, pHandle->height};
}

static void GLFWDisplayDriver_OnWindowResize(GLFWwindow* pWindow, i32 width, i32 height)
{
	GLFWDriverHandle* pHandle = static_cast<GLFWDriverHandle*>(glfwGetWindowUserPointer(pWindow));
	if (pHandle->onWindowResizeCallback)
	{
		pHandle->onWindowResizeCallback(static_cast<u32>(width), static_cast<u32>(height), pHandle->pUserData);
	}
}

static Result GLFWDisplayDriver_SetOnWindowResizeCallback(Pointer<void>			 pWindowHandle,
														  OnWindowResizeCallback callback,
														  void*					 pUserData)
{
	GLFWDriverHandle* pHandle		= CAST_DRIVER_HANDLE(pWindowHandle);
	pHandle->onWindowResizeCallback = callback;
	pHandle->pUserData				= pUserData;

	glfwMakeContextCurrent(pHandle->pWindow);
	glfwSetWindowUserPointer(pHandle->pWindow, pHandle);
	glfwSetWindowSizeCallback(pHandle->pWindow, GLFWDisplayDriver_OnWindowResize);
	glfwSetWindowSize(pHandle->pWindow, static_cast<i32>(pHandle->width), static_cast<i32>(pHandle->height));
	return RESULT_SUCCESS;
}

} // namespace ntt
