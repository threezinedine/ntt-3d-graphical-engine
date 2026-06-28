#include "ntt_glfw_driver.h"
#include "core.h"

#if NTT_PLATFORM_WEB
#include <GLES3/gl3.h>
#else // NTT_PLATFORM_WEB
#include "glad/glad.h"
#endif // NTT_PLATFORM_WEB

#include <GLFW/glfw3.h>

namespace ntt {

void*		g_DefaultGLFWDriverHandle = nullptr;
static bool s_FirstWindowRequired	  = false;

static Result GLFWDisplayDriver_Initialize();
static Result GLFWDisplayDriver_Shutdown();
static Result GLFWDisplayDriver_CreateWindow(u32 width, u32 height, const char* title, void** pWindowHandle);
static bool	  GLFWDisplayDriver_ShouldCloseWindow(void* pWindowHandle);
static Result GLFWDisplayDriver_DestroyWindow(void* pWindowHandle);
static Result GLFWDisplayDriver_OnBeginFrame(void* pDriverHandle);
static Result GLFWDisplayDriver_OnEndFrame(void* pDriverHandle);

Result RegisterGLFWDisplayDriver()
{
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
	s_FirstWindowRequired = false;

	if (!glfwInit())
	{
		NTT_DISPLAY_ERROR("Failed to initialize GLFW.");
		return RESULT_GLFW_INIT_FAILED;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* pWindow = glfwCreateWindow(500, 600, "NTT GLFW Driver", nullptr, nullptr);
	if (pWindow == nullptr)
	{
		NTT_DISPLAY_ERROR("Failed to create GLFW window for driver initialization.");
		glfwTerminate();
		return RESULT_GLFW_WINDOW_CREATION_FAILED;
	}

	glfwMakeContextCurrent(pWindow);

	g_DefaultGLFWDriverHandle = (void*)pWindow;

	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_Shutdown()
{
	glfwTerminate();
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_CreateWindow(u32 width, u32 height, const char* title, void** pWindowHandle)
{
	if (!s_FirstWindowRequired)
	{
		s_FirstWindowRequired = true;
		*pWindowHandle		  = g_DefaultGLFWDriverHandle;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	return RESULT_SUCCESS;
}

static Result GLFWDisplayDriver_OnEndFrame(void* pDriverHandle)
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(pDriverHandle));
	// For GLFW, we don't have a specific driver handle to use here.
	// This function can be used for any per-frame cleanup if needed.
	return RESULT_SUCCESS;
}

} // namespace ntt
