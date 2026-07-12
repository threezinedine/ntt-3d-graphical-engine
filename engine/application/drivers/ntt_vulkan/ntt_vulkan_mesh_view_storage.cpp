#include "ntt_vulkan_mesh_view_storage.h"

namespace ntt {

VulkanMeshViewStorage::VulkanMeshViewStorage(IAllocator* pAllocator)
	: MeshViewStorage(pAllocator)
{
}

VulkanMeshViewStorage::~VulkanMeshViewStorage()
{
}

Result VulkanMeshViewStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanMeshViewStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanMeshViewStorage::AddMeshViewImpl(const Pointer<void>& pMeshViewHandle,
											  const Pointer<void>& pMeshHandle,
											  const Pointer<void>& pRenderContext)
{
	NTT_UNUSED(pMeshViewHandle);
	NTT_UNUSED(pMeshHandle);
	NTT_UNUSED(pRenderContext);

	return RESULT_SUCCESS;
}

Result VulkanMeshViewStorage::RemoveMeshViewImpl(const Pointer<void>& pMeshViewHandle)
{
	NTT_UNUSED(pMeshViewHandle);

	return RESULT_SUCCESS;
}

Result VulkanMeshViewStorage::DrawMeshViewImpl(const Pointer<void>& pMeshViewHanle,
											   const Pointer<void>& pMeshHandle,
											   const Pointer<void>& pRenderContext)
{
	NTT_UNUSED(pMeshViewHanle);
	NTT_UNUSED(pMeshHandle);
	NTT_UNUSED(pRenderContext);

	return RESULT_SUCCESS;
}

Result VulkanMeshViewStorage::DrawDebugLineImpl(const Pointer<void>& pMeshViewHandle,
												const Pointer<void>& pMeshHandle,
												const Pointer<void>& pRenderContext,
												f32					 lineWidth)
{
	NTT_UNUSED(pMeshViewHandle);
	NTT_UNUSED(pMeshHandle);
	NTT_UNUSED(pRenderContext);
	NTT_UNUSED(lineWidth);

	return RESULT_SUCCESS;
}

u32 VulkanMeshViewStorage::GetMeshViewHandleSize() const
{
	return (u32)sizeof(MeshViewHandle);
}

} // namespace ntt
