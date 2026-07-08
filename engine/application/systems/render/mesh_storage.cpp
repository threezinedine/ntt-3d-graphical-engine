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

MeshID MeshStorage::AddMesh(Mesh&& mesh, RenderContextID renderContextID, bool dynamic) noexcept
{
	MeshID	  meshID   = m_pMeshStorage->Add();
	MeshNode* pNode	   = m_pMeshStorage->Get(meshID);
	pNode->mesh		   = static_cast<Mesh&&>(mesh);
	pNode->pMeshHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetMeshHandleSize());
	pNode->shaderID	   = g_DefaultMeshShaderID;
#if NTT_DEBUG
	pNode->debugLineShaderID = g_DebugLineShaderID;
	pNode->debugLineColor	 = Color{1.0f, 0.0f, 0.0f, 1.0f}; // Default debug line color (red)
#endif														  // NTT_DEBUG
	pNode->dynamic = dynamic;
	pNode->pRenderContext =
		SystemGlobals::pRenderSystem->m_pRenderContextStorage->Get(renderContextID)->pRenderContextHandle;

	NTT_ASSERT(AddMeshImpl(pNode->mesh, pNode->pMeshHandle, pNode->pRenderContext, pNode->dynamic) == RESULT_SUCCESS);

	return meshID;
}

Result MeshStorage::DrawMesh(MeshID meshID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->UseShader(pNode->shaderID));

	return DrawMeshImpl(pNode->pMeshHandle, pNode->pRenderContext);
}

#if NTT_DEBUG
Result MeshStorage::DrawDebugLine(MeshID meshID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(
		g_RenderGlobals.pShaderStorage->SetUniformFloat4(pNode->debugLineShaderID, "uColor", pNode->debugLineColor));

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->UseShader(pNode->debugLineShaderID));

	return DrawDebugLineImpl(pNode->pMeshHandle, pNode->pRenderContext);
}
#endif // NTT_DEBUG

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

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
	Result MeshStorage::SetUniform##typeName(MeshID meshID, const char* pUniformName, type value)                      \
	{                                                                                                                  \
		MeshNode* pNode = m_pMeshStorage->Get(meshID);                                                                 \
		if (pNode == nullptr)                                                                                          \
		{                                                                                                              \
			return RESULT_INDEX_OUT_OF_BOUNDS;                                                                         \
		}                                                                                                              \
		ShaderID shaderID = pNode->shaderID;                                                                           \
		NTT_ASSERT_RESULT_SUCCESS(                                                                                     \
			g_RenderGlobals.pShaderStorage->SetUniform##typeName(shaderID, pUniformName, value));                      \
		return RESULT_SUCCESS;                                                                                         \
	}
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF

Result MeshStorage::SetShader(MeshID meshID, ShaderID shaderID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	pNode->shaderID = shaderID;
	return RESULT_SUCCESS;
}

#if NTT_DEBUG
Result MeshStorage::SetDebugLineShader(MeshID meshID, ShaderID shaderID)
{
	MeshNode* pNode = m_pMeshStorage->Get(meshID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	pNode->debugLineShaderID = shaderID;
	return RESULT_SUCCESS;
}
#endif // NTT_DEBUG

} // namespace ntt