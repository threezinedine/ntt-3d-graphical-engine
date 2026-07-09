#pragma once

#if NTT_PLATFORM_WEB
#include <GLES3/gl3.h>
#else // NTT_PLATFORM_WEB
#include "glad/glad.h"
#endif // NTT_PLATFORM_WEB
#include "GLFW/glfw3.h"

#if NTT_ENABLE_ASSERTION
#define GL_ASSERT(exp)                                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		while (glGetError() != GL_NO_ERROR)                                                                            \
		{                                                                                                              \
		}                                                                                                              \
		(exp);                                                                                                         \
		GLenum err = glGetError();                                                                                     \
		NTT_ASSERT_MSG(err == GL_NO_ERROR, "OpenGL error: %d", err);                                                   \
	} while (0)
#else // NTT_ENABLE_ASSERTION
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
#endif // NTT_ENABLE_ASSERTION

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
	u32 verticesCount;
	u32 vao;
	u32 vbo;
#if NTT_DEBUG
	u32 debugVAO;
	u32 debugVBO;
#endif // NTT_DEBUG
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

struct TextureHandle
{
	u32 textureID;
};

#define CAST_TEXTURE_HANDLE(handle)                                                                                    \
	reinterpret_cast<TextureHandle*>(handle.Get());                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

struct UniformInfo
{
	u32 slot;
};

#define CAST_UNIFORM_INFO(handle)                                                                                      \
	reinterpret_cast<UniformInfo*>(handle.Get());                                                                      \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

} // namespace ntt
