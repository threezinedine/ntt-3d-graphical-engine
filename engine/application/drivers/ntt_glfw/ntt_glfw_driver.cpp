#include "ntt_glfw_driver.h"
#include "core.h"

#if NTT_PLATFORM_WEB
#include <GLES3/gl3.h>
#else // NTT_PLATFORM_WEB
#include "glad/glad.h"
#endif // NTT_PLATFORM_WEB

#include <GLFW/glfw3.h>

namespace ntt {

GLFWwindow* g_pDefaultWindow	  = nullptr;
static bool s_FirstWindowRequired = false;

struct GLFWDriverHandle
{
	GLFWwindow* pWindow;
};

static Result GLFWDisplayDriver_Initialize();
static Result GLFWDisplayDriver_Shutdown();
static Result GLFWDisplayDriver_CreateWindow(u32 width, u32 height, const char* title, Pointer<void>& pWindowHandle);
static bool	  GLFWDisplayDriver_ShouldCloseWindow(Pointer<void> pWindowHandle);
static Result GLFWDisplayDriver_DestroyWindow(Pointer<void> pWindowHandle);
static Result GLFWDisplayDriver_OnBeginFrame(Pointer<void> pDriverHandle);
static Result GLFWDisplayDriver_OnEndFrame(Pointer<void> pDriverHandle);
static u32	  GLFWDisplayDriver_GetWindowHandleSize();

Result RegisterGLFWDisplayDriver()
{
	g_DisplayDriver.Initialize			= GLFWDisplayDriver_Initialize;
	g_DisplayDriver.Shutdown			= GLFWDisplayDriver_Shutdown;
	g_DisplayDriver.CreateWindow		= GLFWDisplayDriver_CreateWindow;
	g_DisplayDriver.ShouldCloseWindow	= GLFWDisplayDriver_ShouldCloseWindow;
	g_DisplayDriver.DestroyWindow		= GLFWDisplayDriver_DestroyWindow;
	g_DisplayDriver.OnBeginFrame		= GLFWDisplayDriver_OnBeginFrame;
	g_DisplayDriver.OnEndFrame			= GLFWDisplayDriver_OnEndFrame;
	g_DisplayDriver.GetWindowHandleSize = GLFWDisplayDriver_GetWindowHandleSize;
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_Initialize()
{
	s_FirstWindowRequired = false;

	if (!glfwInit())
	{
		NTT_DISPLAY_ERROR("Failed to initialize GLFW.");
		return RESULT_GLFW_INIT_FAILED;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	NTT_DISPLAY_INFO("GLFW loaded with OpenGL version: %s", glfwGetVersionString());

	GLFWwindow* pWindow = glfwCreateWindow(500, 600, "NTT GLFW Driver", nullptr, nullptr);
	if (pWindow == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to create GLFW window for driver initialization.");
		glfwTerminate();
		return RESULT_GLFW_WINDOW_CREATION_FAILED;
	}

	glfwMakeContextCurrent(pWindow);

	g_pDefaultWindow = pWindow;

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_Shutdown()
{
	glfwTerminate();
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_CreateWindow(u32 width, u32 height, const char* title, Pointer<void>& pWindowHandle)
{
	Pointer<GLFWDriverHandle> pHandle = pWindowHandle.Cast<GLFWDriverHandle>();

	if (!s_FirstWindowRequired)
	{
		s_FirstWindowRequired = true;
		pHandle->pWindow	  = g_pDefaultWindow;
		return RESULT_SUCCESS;
	}

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

	pHandle->pWindow = pWindow;

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
	NTT_UNUSED(pDriverHandle);
	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_OnEndFrame(Pointer<void> pDriverHandle)
{
#if 0
	NTT_UNUSED(pDriverHandle);
#else
	Pointer<GLFWDriverHandle> pHandle = pDriverHandle.Cast<GLFWDriverHandle>();

	if (pHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GLFWwindow* pWindow = static_cast<GLFWwindow*>(pHandle->pWindow);
	glfwSwapBuffers(pWindow);
#endif
	return RESULT_SUCCESS;
}

static u32 GLFWDisplayDriver_GetWindowHandleSize()
{
	return (u32)sizeof(GLFWDriverHandle);
}

} // namespace ntt
