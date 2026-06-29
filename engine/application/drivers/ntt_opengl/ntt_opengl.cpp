#include "ntt_opengl.h"

#include "ntt_opengl_inc.h"

#include "ntt_opengl_mesh_storage.h"
#include "ntt_opengl_shader_storage.h"
#include "systems/render/render_globals.h"

namespace ntt {

struct OpenGLDriverHandle
{
	GLFWwindow* pWindow;
};

static Result OpenGLDriver_Initialize();
static Result OpenGLDriver_Shutdown();
static Result OpenGLDriver_StartRender(Pointer<void> pDriverHandle);
static Result OpenGLDriver_EndRender(Pointer<void> pDriverHandle);
static Result OpenGLDriver_Present(Pointer<void> pDriverHandle);
static u32	  OpenGLDriver_GetRenderDriverHandleSize();

Result RegisterOpenGLDriver()
{
	g_RenderDriver.Initialize				 = OpenGLDriver_Initialize;
	g_RenderDriver.Shutdown					 = OpenGLDriver_Shutdown;
	g_RenderDriver.StartRender				 = OpenGLDriver_StartRender;
	g_RenderDriver.EndRender				 = OpenGLDriver_EndRender;
	g_RenderDriver.Present					 = OpenGLDriver_Present;
	g_RenderDriver.GetRenderDriverHandleSize = OpenGLDriver_GetRenderDriverHandleSize;

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
	return RESULT_SUCCESS;
}

static Result OpenGLDriver_EndRender(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	return RESULT_SUCCESS;
}

static Result OpenGLDriver_Present(Pointer<void> pDriverHandle)
{
#if 0
	Pointer<OpenGLDriverHandle> pHandle = pDriverHandle.Cast<OpenGLDriverHandle>();
	if (pHandle == nullptr || pHandle.Get() == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	glfwSwapBuffers(pHandle->pWindow);
#else
	NTT_UNUSED(pDriverHandle);
#endif
	return RESULT_SUCCESS;
}

static u32 OpenGLDriver_GetRenderDriverHandleSize()
{
	return (u32)sizeof(OpenGLDriverHandle);
}

} // namespace ntt
