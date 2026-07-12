#pragma once
#include "ntt_opengl_inc.h"
#include "systems/render/mesh_view_storage.h"

namespace ntt {

class OpenGLMeshViewStorage : public MeshViewStorage
{
public:
	OpenGLMeshViewStorage(IAllocator* pAllocator = nullptr);
	~OpenGLMeshViewStorage();

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
