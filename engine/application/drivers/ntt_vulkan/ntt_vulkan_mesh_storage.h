#if NTT_VULKAN

#pragma once

#include "systems/render/mesh_storage.h"

namespace ntt {

class VulkanMeshStorage : public MeshStorage
{
public:
	VulkanMeshStorage(IAllocator* pAllocator = nullptr);
	~VulkanMeshStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result
	AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext, bool dynamic) override;
	virtual Result DrawMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext) override;
#if NTT_DEBUG
	virtual Result
	DrawDebugLineImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext, u32 lineWidth) override;
#endif // NTT_DEBUG
	virtual Result RemoveMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext) override;

protected:
	virtual u32 GetMeshHandleSize() const override;
};

} // namespace ntt

#endif // NTT_VULKAN