#if NTT_VULKAN

#include "systems/render/shader_storage.h"

namespace ntt {

class VulkanShaderStorage : public ShaderStorage
{
public:
	VulkanShaderStorage(IAllocator* pAllocator = nullptr);
	~VulkanShaderStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result AddShaderImpl(const Pointer<void>& pRenderContext,
								 ShaderInputTopology  inputTopology,
								 const char*		  pVertexShaderSource,
								 const char*		  pFragmentShaderSource,
								 Pointer<void>&		  pShaderHandle,
								 Uniform*			  pUniforms,
								 u32&				  uniformCount) override;
	virtual Result UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) override;
	virtual Result RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) override;

protected:
	u32 GetUniformInfoSize() const override;

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
protected:                                                                                                             \
	virtual Result SetUniform##typeName##Impl(                                                                         \
		const Uniform& uniform, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderContext) override;
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF

protected:
	virtual u32 GetShaderHandleSize() const override;
};

} // namespace ntt

#endif // NTT_VULKAN