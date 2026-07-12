#if NTT_VULKAN

#pragma once
#include "ntt_vulkan_inc.h"
#include "systems/render/mesh_view_storage.h"

namespace ntt {

class VulkanMeshViewStorage : public MeshViewStorage
{
public:
	VulkanMeshViewStorage(IAllocator* pAllocator = nullptr);
	~VulkanMeshViewStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result AddMeshViewImpl(const Pointer<void>& pMeshViewHandle,
								   const Pointer<void>& pMeshHandle,
								   const Pointer<void>& pRenderContext) override;

	virtual Result RemoveMeshViewImpl(const Pointer<void>& pMeshViewHandle) override;

	virtual Result DrawMeshViewImpl(const Pointer<void>& pMeshViewHanle,
									const Pointer<void>& pMeshHandle,
									const Pointer<void>& pRenderContext) override;

	virtual Result DrawDebugLineImpl(const Pointer<void>& pMeshViewHandle,
									 const Pointer<void>& pMeshHandle,
									 const Pointer<void>& pRenderContext,
									 f32				  lineWidth) override;

	virtual u32 GetMeshViewHandleSize() const override;
};

} // namespace ntt

#endif // NTT_VULKAN