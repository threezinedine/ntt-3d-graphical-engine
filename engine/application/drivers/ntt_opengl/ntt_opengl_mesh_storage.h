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

	virtual Result AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle, bool dynamic) override;
	virtual Result RemoveMeshImpl(const Pointer<void>& pMeshHandle) override;

protected:
	virtual u32 GetMeshHandleSize() const override;
};

} // namespace ntt
