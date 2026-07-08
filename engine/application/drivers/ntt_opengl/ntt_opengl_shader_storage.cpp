#include "ntt_opengl_shader_storage.h"
#include "ntt_opengl_inc.h"

namespace {

static void setUniformValue(GLint location, float value)
{
	glUniform1f(location, value);
}

static void setUniformValue(GLint location, ntt::Vec2f value)
{
	glUniform2f(location, value[0], value[1]);
}

static void setUniformValue(GLint location, ntt::Vec3f value)
{
	glUniform3f(location, value[0], value[1], value[2]);
}

static void setUniformValue(GLint location, ntt::Vec4f value)
{
	glUniform4f(location, value[0], value[1], value[2], value[3]);
}

} // anonymous namespace

namespace ntt {

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

static Result extractUniforms(u32 shaderProgram, Uniform* pUniforms, u32& uniformCount);

Result OpenGLShaderStorage::AddShaderImpl(const Pointer<void>& pRenderContext,
										  const char*		   pVertexShaderSource,
										  const char*		   pFragmentShaderSource,
										  Pointer<void>&	   pShaderHandle,
										  Uniform*			   pUniforms,
										  u32&				   uniformCount)
{
	NTT_UNUSED(pRenderContext);

	ShaderHandle* pHandle = CAST_SHADER_HANDLE(pShaderHandle);

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

	NTT_ASSERT_RESULT_SUCCESS(extractUniforms(pHandle->program, pUniforms, uniformCount));

	return RESULT_SUCCESS;
}

static Result extractUniforms(u32 shaderProgram, Uniform* pUniforms, u32& uniformCount)
{
	i32 count = 0;
	GL_ASSERT(glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &count));

	for (i32 i = 0; i < count; ++i)
	{
		char	name[256];
		GLsizei length;
		GLint	size;
		GLenum	type;

		GL_ASSERT(glGetActiveUniform(shaderProgram, i, sizeof(name), &length, &size, &type, name));

		Uniform& uniform = pUniforms[uniformCount++];
		uniform.name	 = String(name, g_GlobalAllocators.pMalloc);
#define UNIFORM_TYPE_DEF(typeName, name, uppercase, glType)                                                            \
	if (type == glType)                                                                                                \
	{                                                                                                                  \
		uniform.type = UNIFORM_TYPE_##uppercase;                                                                       \
	}
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF
		uniform.value		  = {};		 // Initialize the value based on the type
		uniform.pInternalData = nullptr; // Set internal data if needed
	}

	return RESULT_SUCCESS;
}

#define UNIFORM_TYPE_DEF(type, _name, uppercase, glType)                                                               \
	Result OpenGLShaderStorage::SetUniform##_name##Impl(                                                               \
		const Uniform& uniform, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderContext)               \
	{                                                                                                                  \
		NTT_UNUSED(pRenderContext);                                                                                    \
		ShaderHandle* pHandle = CAST_SHADER_HANDLE(pShaderHandle);                                                     \
		if (!pHandle)                                                                                                  \
		{                                                                                                              \
			return RESULT_UNKNOWN;                                                                                     \
		}                                                                                                              \
                                                                                                                       \
		GLint location = glGetUniformLocation(pHandle->program, uniform.name.CStr());                                  \
		if (location == -1)                                                                                            \
		{                                                                                                              \
			return RESULT_UNKNOWN;                                                                                     \
		}                                                                                                              \
                                                                                                                       \
		GL_ASSERT(glUseProgram(pHandle->program));                                                                     \
		GL_ASSERT(setUniformValue(location, uniform.value._name));                                                     \
                                                                                                                       \
		return RESULT_SUCCESS;                                                                                         \
	}
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF

Result OpenGLShaderStorage::UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(pRenderContext);
	ShaderHandle* pHandle = CAST_SHADER_HANDLE(pShaderHandle);

	GL_ASSERT(glUseProgram(pHandle->program));

	return RESULT_SUCCESS;
}

Result OpenGLShaderStorage::RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(pRenderContext);
	ShaderHandle* pHandle = CAST_SHADER_HANDLE(pShaderHandle);

	GL_ASSERT(glDeleteProgram(pHandle->program));

	return RESULT_SUCCESS;
}

u32 OpenGLShaderStorage::GetShaderHandleSize() const
{
	return (u32)sizeof(ShaderHandle);
}

} // namespace ntt
