#include "ntt_vulkan_mesh_storage.h"

namespace ntt {

struct MeshHandle
{
	u32 vertexBuffer;
	u32 indexBuffer;
	u32 vertexCount;
	u32 indexCount;
};

VulkanMeshStorage::VulkanMeshStorage(IAllocator* pAllocator)
	: MeshStorage(pAllocator)
{
}

VulkanMeshStorage::~VulkanMeshStorage()
{
}

Result VulkanMeshStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle)
{
	NTT_UNUSED(mesh);
	NTT_UNUSED(pMeshHandle);
	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::DrawMeshImpl(const Pointer<void>& pMeshHandle)
{
	NTT_UNUSED(pMeshHandle);
	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::RemoveMeshImpl(const Pointer<void>& pMeshHandle)
{
	NTT_UNUSED(pMeshHandle);
	return RESULT_SUCCESS;
}

u32 VulkanMeshStorage::GetMeshHandleSize() const
{
	return (u32)sizeof(MeshHandle);
}

} // namespace ntt
