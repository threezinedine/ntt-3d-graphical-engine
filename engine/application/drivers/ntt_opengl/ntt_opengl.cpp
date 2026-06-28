#include "ntt_opengl.h"

#if NTT_PLATFORM_WEB
#include <GLES3/gl3.h>
#else // NTT_PLATFORM_WEB
#include "glad/glad.h"
#endif // NTT_PLATFORM_WEB
#include "GLFW/glfw3.h"

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

	return RESULT_SUCCESS;
}

static Result OpenGLDriver_Shutdown()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
