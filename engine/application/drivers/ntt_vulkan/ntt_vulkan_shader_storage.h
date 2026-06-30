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
								 const char*		  pVertexShaderSource,
								 const char*		  pFragmentShaderSource,
								 Pointer<void>&		  pShaderHandle) override;
	virtual Result UseShaderImpl(const Pointer<void>& pShaderHandle) override;
	virtual Result RemoveShaderImpl(const Pointer<void>& pShaderHandle) override;

protected:
	virtual u32 GetShaderHandleSize() const override;
};

} // namespace ntt

#endif // NTT_VULKAN