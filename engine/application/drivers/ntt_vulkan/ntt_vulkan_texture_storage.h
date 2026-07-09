#if NTT_VULKAN

#include "systems/render/texture_storage.h"

namespace ntt {

class VulkanTextureStorage : public TextureStorage
{
public:
	VulkanTextureStorage(IAllocator* pAllocator = nullptr);
	~VulkanTextureStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result
	AddTextureImpl(Vec3i textureShape, u8* pData, Pointer<void>& pTextureHandle, const TextureSample& sample) override;
	virtual Result SetTextureSampleImpl(TextureID textureID, const TextureSample& sample) override;
	virtual Result RemoveTextureImpl(const Pointer<void>& pTextureHandle) override;

	virtual u32 GetTextureHandleSize() const override;
};

} // namespace ntt

#endif // NTT_VULKAN