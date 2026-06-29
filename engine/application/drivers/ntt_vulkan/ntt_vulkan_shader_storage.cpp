#include "ntt_vulkan_shader_storage.h"

namespace ntt {

struct ShaderHandle
{
	u32 program;
};

VulkanShaderStorage::VulkanShaderStorage(IAllocator* pAllocator)
	: ShaderStorage(pAllocator)
{
}

VulkanShaderStorage::~VulkanShaderStorage()
{
}

Result VulkanShaderStorage::InitializeImpl()
{
	// Implementation for Vulkan-specific initialization
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::ShutdownImpl()
{
	// Implementation for Vulkan-specific shutdown
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::AddShaderImpl(const char*	 pVertexShaderSource,
										  const char*	 pFragmentShaderSource,
										  Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(pVertexShaderSource);
	NTT_UNUSED(pFragmentShaderSource);
	NTT_UNUSED(pShaderHandle);
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::UseShaderImpl(const Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(pShaderHandle);
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::RemoveShaderImpl(const Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(pShaderHandle);

	return RESULT_SUCCESS;
}

u32 VulkanShaderStorage::GetShaderHandleSize() const
{
	return (u32)sizeof(ShaderHandle);
}

} // namespace ntt
