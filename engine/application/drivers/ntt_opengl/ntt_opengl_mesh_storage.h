#pragma once

#include "systems/render/mesh_storage.h"

namespace ntt {

class OpenGLMeshStorage : public MeshStorage
{
public:
	OpenGLMeshStorage(IAllocator* pAllocator = nullptr);
	~OpenGLMeshStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result
	AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext, bool dynamic) override;
	virtual Result DrawMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext) override;
#if NTT_DEBUG
	virtual Result DrawDebugLineImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext) override;
#endif // NTT_DEBUG
	virtual Result RemoveMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext) override;

protected:
	virtual u32 GetMeshHandleSize() const override;
};

} // namespace ntt
