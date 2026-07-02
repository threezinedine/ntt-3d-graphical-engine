#include "mesh_storage.h"
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

MeshID MeshStorage::AddMesh(Mesh&& mesh, RenderContextID renderContextID, bool dynamic) noexcept
{
	MeshNode node;
	node.mesh		 = static_cast<Mesh&&>(mesh);
	node.pMeshHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetMeshHandleSize());
	node.dynamic	 = dynamic;
	node.pRenderContext =
		SystemGlobals::pRenderSystem->m_pRenderContextStorage->Get(renderContextID)->pRenderContextHandle;

	NTT_ASSERT(AddMeshImpl(node.mesh, node.pMeshHandle, node.pRenderContext, node.dynamic) == RESULT_SUCCESS);

	return m_pMeshStorage->Add(static_cast<MeshNode&&>(node));
}

Result MeshStorage::DrawMesh(MeshID meshID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	return DrawMeshImpl(pNode->pMeshHandle, pNode->pRenderContext);
}

Result MeshStorage::RemoveMesh(MeshID meshID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(RemoveMeshImpl(pNode->pMeshHandle, pNode->pRenderContext));
	NTT_ASSERT_RESULT_SUCCESS(pNode->pMeshHandle.Free());

	return m_pMeshStorage->Remove(meshID);
}

} // namespace ntt