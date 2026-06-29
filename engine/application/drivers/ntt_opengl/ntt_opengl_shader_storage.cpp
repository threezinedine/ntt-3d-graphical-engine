#include "ntt_opengl_shader_storage.h"
#include "ntt_opengl_inc.h"

namespace ntt {

struct ShaderHandle
{
	u32 program;
};

OpenGLShaderStorage::OpenGLShaderStorage(IAllocator* pAllocator)
	: ShaderStorage(pAllocator)
{
}

OpenGLShaderStorage::~OpenGLShaderStorage()
{
}

Result OpenGLShaderStorage::InitializeImpl()
{
	// Implementation for OpenGL-specific initialization
	return RESULT_SUCCESS;
}

Result OpenGLShaderStorage::ShutdownImpl()
{
	// Implementation for OpenGL-specific shutdown
	return RESULT_SUCCESS;
}

Result OpenGLShaderStorage::AddShaderImpl(const char*	 pVertexShaderSource,
										  const char*	 pFragmentShaderSource,
										  Pointer<void>& pShaderHandle)
{
	if (pShaderHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	ShaderHandle* pHandle = reinterpret_cast<ShaderHandle*>(pShaderHandle.Get());

	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GL_ASSERT(glShaderSource(vertexShader, 1, &pVertexShaderSource, nullptr));
	GL_ASSERT(glCompileShader(vertexShader));

	// Check for compilation errors
	GLint success;
	GL_ASSERT(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		char infoLog[512];
		GL_ASSERT(glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog));
		NTT_RENDER_ERROR("Vertex shader compilation failed: %s", infoLog);
		// Handle the error (e.g., log it, return an error code)
		return RESULT_SHADER_COMPILATION_FAILED; // Return an appropriate error code
	}

	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GL_ASSERT(glShaderSource(fragmentShader, 1, &pFragmentShaderSource, nullptr));
	GL_ASSERT(glCompileShader(fragmentShader));

	// Check for compilation errors
	GL_ASSERT(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		char infoLog[512];
		GL_ASSERT(glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog));
		NTT_RENDER_ERROR("Fragment shader compilation failed: %s", infoLog);
		// Handle the error (e.g., log it, return an error code)
		return RESULT_SHADER_COMPILATION_FAILED; // Return an appropriate error code
	}

	pHandle->program = glCreateProgram();
	GL_ASSERT(glAttachShader(pHandle->program, vertexShader));
	GL_ASSERT(glAttachShader(pHandle->program, fragmentShader));
	GL_ASSERT(glLinkProgram(pHandle->program));

	// Check for linking errors
	GL_ASSERT(glGetProgramiv(pHandle->program, GL_LINK_STATUS, &success));
	if (!success)
	{
		char infoLog[512];
		GL_ASSERT(glGetProgramInfoLog(pHandle->program, 512, nullptr, infoLog));
		NTT_RENDER_ERROR("Shader program linking failed: %s", infoLog);
		// Handle the error (e.g., log it, return an error code)
		return RESULT_SHADER_LINKING_FAILED; // Return an appropriate error code
	}

	// Clean up shaders as they are no longer needed after linking
	GL_ASSERT(glDeleteShader(vertexShader));
	GL_ASSERT(glDeleteShader(fragmentShader));

	return RESULT_SUCCESS;
}

Result OpenGLShaderStorage::UseShaderImpl(const Pointer<void>& pShaderHandle)
{
	ShaderHandle* pHandle = reinterpret_cast<ShaderHandle*>(pShaderHandle.Get());

	GL_ASSERT(glUseProgram(pHandle->program));

	return RESULT_SUCCESS;
}

Result OpenGLShaderStorage::RemoveShaderImpl(const Pointer<void>& pShaderHandle)
{
	ShaderHandle* pHandle = reinterpret_cast<ShaderHandle*>(pShaderHandle.Get());

	GL_ASSERT(glDeleteProgram(pHandle->program));

	return RESULT_SUCCESS;
}

u32 OpenGLShaderStorage::GetShaderHandleSize() const
{
	return (u32)sizeof(ShaderHandle);
}

} // namespace ntt
