#include "ntt_opengl.h"

#include "ntt_opengl_inc.h"

#include "ntt_opengl_mesh_storage.h"
#include "ntt_opengl_shader_storage.h"
#include "systems/render/render_globals.h"

namespace ntt {

static Result OpenGLDriver_Initialize();
static Result OpenGLDriver_Shutdown();

Result RegisterOpenGLDriver()
{
	g_RenderDriver.Initialize = OpenGLDriver_Initialize;
	g_RenderDriver.Shutdown	  = OpenGLDriver_Shutdown;

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

} // namespace ntt
