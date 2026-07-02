#pragma once

#include "components/components.h"
#include "services.h"
#include "systems/render/render_system.h"

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

	MeshID AddMesh(Mesh&& mesh, RenderContextID renderContextID) noexcept;
	Result DrawMesh(MeshID meshID);
	Result RemoveMesh(MeshID meshID);

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle)								   = 0;
	virtual Result DrawMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext) = 0;
	virtual Result RemoveMeshImpl(const Pointer<void>& pMeshHandle)									   = 0;

protected:
	virtual u32 GetMeshHandleSize() const = 0;

private:
	struct MeshNode
	{
		Mesh		  mesh;
		Pointer<void> pMeshHandle;
		Pointer<void> pRenderContext;
	};

	IAllocator*				 m_pAllocator;
	Scope<Storage<MeshNode>> m_pMeshStorage;
};

} // namespace ntt
