#include "mesh_storage.h"
#include "render_globals.h"
#include "render_system.h"
#include "systems/render/render_system.h"
#include "systems/system_globals.h"

namespace ntt {

MeshStorage::MeshStorage(IAllocator* pAllocator)
	: m_pAllocator(pAllocator)
{
}

Result MeshStorage::Initialize()
{
	m_pMeshStorage = MakeScope<Storage<MeshNode>>(m_pAllocator, 30);

	if (m_pMeshStorage == nullptr)
	{
		return RESULT_OUT_OF_MEMORY;
	}

	return InitializeImpl();
}

Result MeshStorage::Shutdown()
{
	for (u32 i = 0; i < m_pMeshStorage->GetCount(); ++i)
	{
		if (m_pMeshStorage->IsActive(i))
		{
			RemoveMesh(i);
		}
	}

	m_pMeshStorage.Reset();
	return ShutdownImpl();
}

MeshStorage::~MeshStorage()
{
}

MeshID MeshStorage::AddMesh(Mesh&& mesh, bool dynamic) noexcept
{
	MeshID	  meshID   = m_pMeshStorage->Add();
	MeshNode* pNode	   = m_pMeshStorage->Get(meshID);
	pNode->mesh		   = static_cast<Mesh&&>(mesh);
	pNode->pMeshHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetMeshHandleSize());
	pNode->dynamic	   = dynamic;
	NTT_ASSERT(AddMeshImpl(pNode->mesh, pNode->pMeshHandle, pNode->dynamic) == RESULT_SUCCESS);

	return meshID;
}

Result MeshStorage::RemoveMesh(MeshID meshID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(RemoveMeshImpl(pNode->pMeshHandle));
	NTT_ASSERT_RESULT_SUCCESS(pNode->pMeshHandle.Free());

	return m_pMeshStorage->Remove(meshID);
}

} // namespace ntt