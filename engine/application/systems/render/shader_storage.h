#pragma once

#include "render_system.h"
#include "render_system_types.h"
#include "services.h"
#include "uniform.h"

namespace ntt {

enum ShaderInputTopology
{
#define SHADER_INPUT_TOPOLOGY_DEF(option, vkTopology, vkRasterizationMode) NTT_SHADER_INPUT_TOPOLOGY_##option,
#include "shader_input_topology.def"
#undef SHADER_INPUT_TOPOLOGY_DEF
};

const char* ShaderInputTopologyToString(ShaderInputTopology topology);
class ShaderStorage
{
public:
	ShaderStorage(IAllocator* pAllocator = nullptr);
	virtual ~ShaderStorage();

	Result Initialize();
	Result Shutdown();

	Result SetupDefaultShaders();
	Result RemoveDefaultShaders();

	ShaderID AddShader(ShaderInputTopology inputTopology,
					   const char*		   pVertexShaderSource,
					   const char*		   pFragmentShaderSource) noexcept;
	Result	 UseShader(ShaderID shaderID, RenderContextID renderContextID);
	Result	 RemoveShader(ShaderID shaderID);

#define UNIFORM_TYPE_SAMPLER_DEF(type, typeName, uppercase, glType)                                                    \
public:                                                                                                                \
	Result SetUniform##typeName(ShaderID shaderID, const char* pUniformName, type value);                              \
                                                                                                                       \
protected:                                                                                                             \
	virtual Result SetUniform##typeName##Impl(const Uniform&	   uniform,                                            \
											  const Pointer<void>& pTextureHandle,                                     \
											  const Pointer<void>& pShaderHandle,                                      \
											  const Pointer<void>& pRenderHandle) = 0;
#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
public:                                                                                                                \
	Result SetUniform##typeName(ShaderID shaderID, const char* pUniformName, type value);                              \
                                                                                                                       \
protected:                                                                                                             \
	virtual Result SetUniform##typeName##Impl(                                                                         \
		const Uniform& uniform, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderHandle) = 0;
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF

protected:
	virtual u32 GetUniformInfoSize() const = 0;

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddShaderImpl(ShaderInputTopology inputTopology,
								 const char*		 pVertexShaderSource,
								 const char*		 pFragmentShaderSource,
								 Pointer<void>&		 pShaderHandle,
								 Uniform*			 pUniforms,
								 u32&				 uniformCount)														  = 0;
	virtual Result UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) = 0;
	virtual Result RemoveShaderImpl(const Pointer<void>& pShaderHandle)									  = 0;

protected:
	virtual u32 GetShaderHandleSize() const = 0;

private:
	struct ShaderNode
	{
		Pointer<void>		pShaderHandle;
		Uniform				uniforms[16];
		u32					uniformCount;
		ShaderInputTopology inputTopology;
	};

private:
	IAllocator*				   m_pAllocator;
	Scope<Storage<ShaderNode>> m_pStorage;
};

} // namespace ntt