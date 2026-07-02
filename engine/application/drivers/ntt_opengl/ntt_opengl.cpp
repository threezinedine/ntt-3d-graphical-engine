#include "ntt_opengl.h"

#include "ntt_opengl_inc.h"

#include "ntt_opengl_mesh_storage.h"
#include "ntt_opengl_shader_storage.h"
#include "systems/display/display_driver.h"
#include "systems/render/render_globals.h"

namespace ntt {

static Result OpenGLDriver_Initialize();
static Result OpenGLDriver_Shutdown();
static Result OpenGLDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle);
static Result OpenGLDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle);
static Result OpenGLDriver_StartRender(Pointer<void> pDriverHandle);
static Result OpenGLDriver_EndRender(Pointer<void> pDriverHandle);
static Result OpenGLDriver_Present(Pointer<void> pDriverHandle);
static u32	  OpenGLDriver_GetRenderContextSize();

static Result OpenGLDriver_OnWindowResize(Pointer<void> pWindowHandle, u32 width, u32 height);

Result RegisterOpenGLDriver()
{
	g_RenderDriver.Initialize				  = OpenGLDriver_Initialize;
	g_RenderDriver.Shutdown					  = OpenGLDriver_Shutdown;
	g_RenderDriver.CreateRenderContext		  = OpenGLDriver_CreateRenderContext;
	g_RenderDriver.DestroyRenderContext		  = OpenGLDriver_DestroyRenderContext;
	g_RenderDriver.StartRender				  = OpenGLDriver_StartRender;
	g_RenderDriver.EndRender				  = OpenGLDriver_EndRender;
	g_RenderDriver.Present					  = OpenGLDriver_Present;
	g_RenderDriver.GetRenderContextHandleSize = OpenGLDriver_GetRenderContextSize;

	g_DisplayDriver.OnWindowResize = OpenGLDriver_OnWindowResize;

	return RESULT_SUCCESS;
}

static Result OpenGLDriver_Initialize()
{
#if !NTT_PLATFORM_WEB
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		NTT_RENDER_ERROR("Failed to initialize OpenGL context.");
		return RESULT_UNKNOWN;
	}
#endif // !NTT_PLATFORM_WEB

	NTT_RENDER_INFO("OpenGL context initialized successfully.");
	return RESULT_SUCCESS;
}

static Result OpenGLDriver_Shutdown()
{
	NTT_RENDER_INFO("OpenGL context shut down successfully.");

	return RESULT_SUCCESS;
}

Result RegisterOpenGLRenderer()
{
	g_RenderGlobals.pMeshStorage   = MakeScope<OpenGLMeshStorage>(g_GlobalAllocators.pMalloc);
	g_RenderGlobals.pShaderStorage = MakeScope<OpenGLShaderStorage>(g_GlobalAllocators.pMalloc);

	return RESULT_SUCCESS;
}

static Result OpenGLDriver_StartRender(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	return RESULT_SUCCESS;
}

static Result OpenGLDriver_EndRender(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	return RESULT_SUCCESS;
}

static Result OpenGLDriver_Present(Pointer<void> pDriverHandle)
{
	OpenGLContextHandle* pHandle = CAST_CONTEXT_HANDLE(pDriverHandle);

	glfwSwapBuffers(pHandle->pWindow);
	return RESULT_SUCCESS;
}

static u32 OpenGLDriver_GetRenderContextSize()
{
	return (u32)sizeof(OpenGLContextHandle);
}

static Result OpenGLDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle)
{
	OpenGLContextHandle* pHandle = CAST_CONTEXT_HANDLE(pRenderContextHandle);
	pHandle->pWindow			 = reinterpret_cast<GLFWwindow*>(g_DisplayDriver.GetWindowHandle(pWindowHandle));

	return RESULT_SUCCESS;
}

static Result OpenGLDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle)
{
	NTT_UNUSED(pRenderContextHandle);
	return RESULT_SUCCESS;
}

static Result OpenGLDriver_OnWindowResize(Pointer<void> pWindowHandle, u32 width, u32 height)
{
	GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(g_DisplayDriver.GetWindowHandle(pWindowHandle));

	glfwMakeContextCurrent(pWindow);
	glViewport(0, 0, width, height);

	return RESULT_SUCCESS;
}

} // namespace ntt
