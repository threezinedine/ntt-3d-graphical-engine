#if NTT_VULKAN

#include "ntt_vulkan_texture_storage.h"
#include "ntt_vulkan_inc.h"

namespace ntt {

VulkanTextureStorage::VulkanTextureStorage(IAllocator* pAllocator)
	: TextureStorage(pAllocator)
{
}

VulkanTextureStorage::~VulkanTextureStorage()
{
}

Result VulkanTextureStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanTextureStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanTextureStorage::AddTextureImpl(Vec3i				 textureShape,
											u8*					 pData,
											Pointer<void>&		 pTextureHandle,
											const TextureSample& sample)
{
	TextureHandle* pHandle = VK_TEXTURE_CAST(pTextureHandle);
	NTT_UNUSED(pHandle);
	NTT_UNUSED(textureShape);
	NTT_UNUSED(pData);
	NTT_UNUSED(sample);

	return RESULT_SUCCESS;
}

Result VulkanTextureStorage::SetTextureSampleImpl(TextureID textureID, const TextureSample& sample)
{
	NTT_UNUSED(textureID);
	NTT_UNUSED(sample);

	return RESULT_SUCCESS;
}

Result VulkanTextureStorage::RemoveTextureImpl(const Pointer<void>& pTextureHandle)
{
	TextureHandle* pHandle = VK_TEXTURE_CAST(pTextureHandle);
	NTT_UNUSED(pHandle);

	return RESULT_SUCCESS;
}

u32 VulkanTextureStorage::GetTextureHandleSize() const
{
	return (u32)sizeof(TextureHandle);
}

} // namespace ntt

#endif // NTT_VULKAN