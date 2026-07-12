#pragma once

#include "components/components.h"
#include "render_system_types.h"
#include "services.h"
#include "shader_storage.h"
#include "systems/render/render_system.h"

namespace ntt {

class MeshViewStorage;

class MeshStorage
{
public:
	MeshStorage(IAllocator* pAllocator = nullptr);
	virtual ~MeshStorage();

	Result Initialize();
	Result Shutdown();

	MeshID AddMesh(Mesh&& mesh, bool dynamic = false) noexcept;
	Result RemoveMesh(MeshID meshID);

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle, bool dynamic) = 0;
	virtual Result RemoveMeshImpl(const Pointer<void>& pMeshHandle)					 = 0;

protected:
	virtual u32 GetMeshHandleSize() const = 0;

public:
	struct MeshNode
	{
		Mesh		  mesh;
		bool		  dynamic;
		Pointer<void> pMeshHandle;
	};

	friend class MeshViewStorage;

private:
	IAllocator*				 m_pAllocator;
	Scope<Storage<MeshNode>> m_pMeshStorage;
};

} // namespace ntt
