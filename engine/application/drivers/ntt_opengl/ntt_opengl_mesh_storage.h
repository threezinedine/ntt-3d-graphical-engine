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

	virtual Result AddMeshImpl(Mesh& mesh, void** pMeshHandle) override;
	virtual Result DrawMeshImpl(void* pMeshHandle) override;
	virtual Result RemoveMeshImpl(void* pMeshHandle) override;
};

} // namespace ntt
