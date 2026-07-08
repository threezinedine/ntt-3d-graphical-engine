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

Result VulkanTextureStorage::AddTextureImpl(u8* pData, u32 size, Pointer<void>& pTextureHandle)
{
	TextureHandle* pHandle = VK_TEXTURE_CAST(pTextureHandle);
	NTT_UNUSED(pHandle);
	NTT_UNUSED(pData);
	NTT_UNUSED(size);

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
