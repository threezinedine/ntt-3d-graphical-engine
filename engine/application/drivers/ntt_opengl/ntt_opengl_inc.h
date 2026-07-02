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
			NTT_OPENGL_ERROR("OpenGL error: %d", err);                                                                 \
			return RESULT_OPENGL_ERROR;                                                                                \
		}                                                                                                              \
	} while (0)

namespace ntt {

struct OpenGLContextHandle
{
	GLFWwindow* pWindow;
};

#define CAST_CONTEXT_HANDLE(handle)                                                                                    \
	reinterpret_cast<OpenGLContextHandle*>(handle.Get());                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

struct ShaderHandle
{
	u32 program;
};

#define CAST_SHADER_HANDLE(handle)                                                                                     \
	reinterpret_cast<ShaderHandle*>(handle.Get());                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

struct MeshHandle
{
	u32 vao;
	u32 vbo;
};

#define CAST_MESH_HANDLE(handle)                                                                                       \
	reinterpret_cast<MeshHandle*>(handle.Get());                                                                       \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

} // namespace ntt
