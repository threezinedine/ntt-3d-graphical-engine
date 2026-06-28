#pragma once

#include "components/components.h"
#include "services.h"

namespace ntt {

typedef u32	 MeshID;
const MeshID INVALID_MESH_ID = static_cast<u32>(-1);

class MeshStorage
{
public:
	MeshStorage(IAllocator* pAllocator = nullptr);
	~MeshStorage();

	Result Initialize();
	Result Shutdown();

	MeshID AddMesh(Mesh&& mesh) noexcept;
	Result DrawMesh(MeshID meshID);
	Result RemoveMesh(MeshID meshID);

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddMeshImpl(Mesh& mesh, void** pMeshHandle) = 0;
	virtual Result DrawMeshImpl(void* pMeshHandle)			   = 0;
	virtual Result RemoveMeshImpl(void* pMeshHandle)		   = 0;

private:
	struct MeshNode
	{
		Mesh  mesh;
		void* pMeshHandle;
	};

	IAllocator*				 m_pAllocator;
	Scope<Storage<MeshNode>> m_pMeshStorage;
};

} // namespace ntt
