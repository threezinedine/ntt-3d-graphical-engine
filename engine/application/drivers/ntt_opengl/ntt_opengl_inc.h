#pragma once

#if NTT_PLATFORM_WEB
#include <GLES3/gl3.h>
#else // NTT_PLATFORM_WEB
#include "glad/glad.h"
#endif // NTT_PLATFORM_WEB
#include "GLFW/glfw3.h"

#define GL_ASSERT(exp)                                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		while (glGetError() != GL_NO_ERROR)                                                                            \
		{                                                                                                              \
		}                                                                                                              \
		(exp);                                                                                                         \
		GLenum err = glGetError();                                                                                     \
		if (err != GL_NO_ERROR)                                                                                        \
		{                                                                                                              \
			NTT_RENDER_ERROR("OpenGL error: %d", err);                                                                 \
			return RESULT_OPENGL_ERROR;                                                                                \
		}                                                                                                              \
	} while (0)

