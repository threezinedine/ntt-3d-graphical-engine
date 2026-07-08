#pragma once

#include "render_system.h"
#include "services.h"
#include "uniform.h"

namespace ntt {

typedef u32		   ShaderID;
constexpr ShaderID INVALID_SHADER_ID = static_cast<u32>(-1);

extern ShaderID g_DefaultMeshShaderID;
#if NTT_DEBUG
extern ShaderID g_DebugLineShaderID;
#endif // NTT_DEBUG

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
	~ShaderStorage();

	Result Initialize();
	Result Shutdown();

	Result SetupDefaultShaders(RenderContextID renderContextID);
	Result RemoveDefaultShaders();

	ShaderID AddShader(RenderContextID	   renderContextID,
					   ShaderInputTopology inputTopology,
					   const char*		   pVertexShaderSource,
					   const char*		   pFragmentShaderSource) noexcept;
	Result	 UseShader(ShaderID shaderID);
	Result	 RemoveShader(ShaderID shaderID);

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
public:                                                                                                                \
	Result SetUniform##typeName(ShaderID shaderID, const char* pUniformName, type value);                              \
                                                                                                                       \
protected:                                                                                                             \
	virtual Result SetUniform##typeName##Impl(                                                                         \
		const Uniform& uniform, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderContext) = 0;
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF

protected:
	virtual u32 GetUniformInfoSize() const = 0;

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddShaderImpl(const Pointer<void>& pRenderContext,
								 ShaderInputTopology  inputTopology,
								 const char*		  pVertexShaderSource,
								 const char*		  pFragmentShaderSource,
								 Pointer<void>&		  pShaderHandle,
								 Uniform*			  pUniforms,
								 u32&				  uniformCount)															 = 0;
	virtual Result UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)	 = 0;
	virtual Result RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) = 0;

protected:
	virtual u32 GetShaderHandleSize() const = 0;

private:
	struct ShaderNode
	{
		Pointer<void>		pShaderHandle;
		RenderContextID		renderContextID;
		Uniform				uniforms[16];
		u32					uniformCount;
		ShaderInputTopology inputTopology;
	};

private:
	IAllocator*				   m_pAllocator;
	Scope<Storage<ShaderNode>> m_pStorage;
};

} // namespace ntt