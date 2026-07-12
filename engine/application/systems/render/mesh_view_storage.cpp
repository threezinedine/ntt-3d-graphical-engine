#include "mesh_view_storage.h"
#include "mesh_storage.h"
#include "render_globals.h"
#include "render_system.h"
#include "systems/render/render_system.h"
#include "systems/system_globals.h"

namespace ntt {

MeshViewStorage::MeshViewStorage(IAllocator* pAllocator)
	: m_pAllocator(pAllocator)
{
}

MeshViewStorage::~MeshViewStorage()
{
}

Result MeshViewStorage::Initialize()
{
	m_pMeshViewStorage = MakeScope<Storage<MeshViewNode>>(m_pAllocator);

	if (m_pMeshViewStorage == nullptr)
	{
		return RESULT_OUT_OF_MEMORY;
	}

	return InitializeImpl();
}

Result MeshViewStorage::Shutdown()
{
	for (u32 i = 0; i < m_pMeshViewStorage->GetCount(); ++i)
	{
		NTT_ASSERT_RESULT_SUCCESS(RemoveMeshView(i));
	}

	m_pMeshViewStorage.Reset();
	return ShutdownImpl();
}

MeshViewID MeshViewStorage::AddMeshView(MeshID meshID, RenderContextID renderContextID)
{
	MeshViewID	  meshViewID = m_pMeshViewStorage->Add();
	MeshViewNode* pNode		 = m_pMeshViewStorage->Get(meshViewID);
	pNode->meshID			 = meshID;
	pNode->renderContextID	 = renderContextID;
	pNode->shaderID			 = defaultMeshShaderID;
#if NTT_DEBUG
	pNode->debugLineShaderID = defaultDebugLineShaderID;
	pNode->lineWidth		 = 1.0f;
#endif // NTT_DEBUG
	pNode->pMeshViewHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetMeshViewHandleSize());

	MeshStorage::MeshNode* pMeshNode = g_RenderGlobals.pMeshStorage->m_pMeshStorage->Get(meshID);
	if (pMeshNode == nullptr)
	{
		return INVALID_MESH_VIEW_ID;
	}

	RenderSystem::RenderContext* pRenderContext =
		SystemGlobals::pRenderSystem->m_pRenderContextStorage->Get(renderContextID);
	if (pRenderContext == nullptr)
	{
		return INVALID_MESH_VIEW_ID;
	}

	NTT_ASSERT_RESULT_SUCCESS(
		AddMeshViewImpl(pNode->pMeshViewHandle, pMeshNode->pMeshHandle, pRenderContext->pRenderContextHandle));

	return meshViewID;
}

Result MeshViewStorage::RemoveMeshView(MeshViewID meshViewID)
{
	MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(RemoveMeshViewImpl(pNode->pMeshViewHandle));
	NTT_ASSERT_RESULT_SUCCESS(pNode->pMeshViewHandle.Free());

	return m_pMeshViewStorage->Remove(meshViewID);
}

Result MeshViewStorage::DrawMeshView(MeshViewID meshViewID)
{
	MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	RenderSystem::RenderContext* pRenderContext =
		SystemGlobals::pRenderSystem->m_pRenderContextStorage->Get(pNode->renderContextID);
	if (pRenderContext == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	MeshStorage::MeshNode* pMeshNode = g_RenderGlobals.pMeshStorage->m_pMeshStorage->Get(pNode->meshID);
	if (pMeshNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->UseShader(pNode->shaderID, pNode->renderContextID));

	return DrawMeshViewImpl(pNode->pMeshViewHandle, pMeshNode->pMeshHandle, pRenderContext->pRenderContextHandle);
}

#if NTT_DEBUG
Result MeshViewStorage::DrawDebugLine(MeshViewID meshViewID)
{
	MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	RenderSystem::RenderContext* pRenderContext =
		SystemGlobals::pRenderSystem->m_pRenderContextStorage->Get(pNode->renderContextID);
	if (pRenderContext == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	MeshStorage::MeshNode* pMeshNode = g_RenderGlobals.pMeshStorage->m_pMeshStorage->Get(pNode->meshID);
	if (pMeshNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(
		g_RenderGlobals.pShaderStorage->UseShader(pNode->debugLineShaderID, pNode->renderContextID));

	return DrawDebugLineImpl(
		pNode->pMeshViewHandle, pMeshNode->pMeshHandle, pRenderContext->pRenderContextHandle, pNode->lineWidth);
}
#endif // NTT_DEBUG

Result MeshViewStorage::SetShader(MeshViewID meshViewID, ShaderID shaderID)
{
	MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	pNode->shaderID = shaderID;
	return RESULT_SUCCESS;
}

#if NTT_DEBUG
Result MeshViewStorage::SetDebugLineShader(MeshViewID meshViewID, ShaderID shaderID)
{
	MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	pNode->debugLineShaderID = shaderID;
	return RESULT_SUCCESS;
}

Result MeshViewStorage::SetDebugLineWidth(MeshViewID meshViewID, f32 lineWidth)
{
	MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	pNode->lineWidth = lineWidth;
	return RESULT_SUCCESS;
}
#endif // NTT_DEBUG

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
	Result MeshViewStorage::SetUniform##typeName(MeshViewID meshViewID, const char* pUniformName, type value)          \
	{                                                                                                                  \
		MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);                                                     \
		if (pNode == nullptr)                                                                                          \
		{                                                                                                              \
			return RESULT_INDEX_OUT_OF_BOUNDS;                                                                         \
		}                                                                                                              \
		ShaderID shaderID = pNode->shaderID;                                                                           \
		NTT_ASSERT_RESULT_SUCCESS(                                                                                     \
			g_RenderGlobals.pShaderStorage->SetUniform##typeName(shaderID, pUniformName, value));                      \
		return RESULT_SUCCESS;                                                                                         \
	}
#undef UNIFORM_TYPE_SAMPLER_DEF
#define UNIFORM_TYPE_SAMPLER_DEF(type, typeName, uppercase, glType) UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
	Result MeshViewStorage::SetDebugLineUniform##typeName(MeshViewID meshViewID, const char* pUniformName, type value) \
	{                                                                                                                  \
		MeshViewNode* pNode = m_pMeshViewStorage->Get(meshViewID);                                                     \
		if (pNode == nullptr)                                                                                          \
		{                                                                                                              \
			return RESULT_INDEX_OUT_OF_BOUNDS;                                                                         \
		}                                                                                                              \
		ShaderID shaderID = pNode->debugLineShaderID;                                                                  \
		NTT_ASSERT_RESULT_SUCCESS(                                                                                     \
			g_RenderGlobals.pShaderStorage->SetUniform##typeName(shaderID, pUniformName, value));                      \
		return RESULT_SUCCESS;                                                                                         \
	}
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF

} // namespace ntt
