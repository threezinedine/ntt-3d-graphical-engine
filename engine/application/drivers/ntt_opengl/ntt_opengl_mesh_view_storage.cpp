#include "ntt_opengl_mesh_view_storage.h"
#include "ntt_opengl_inc.h"
#include "systems/render/mesh_storage.h"

namespace ntt {

OpenGLMeshViewStorage::OpenGLMeshViewStorage(IAllocator* pAllocator)
	: MeshViewStorage(pAllocator)
{
}

OpenGLMeshViewStorage::~OpenGLMeshViewStorage()
{
}

Result OpenGLMeshViewStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result OpenGLMeshViewStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

Result OpenGLMeshViewStorage::AddMeshViewImpl(const Pointer<void>& pMeshViewHandle,
											  const Pointer<void>& pMeshVoidHandle,
											  const Pointer<void>& pRenderContext)
{
	MeshViewHandle*		 pHandle		= CAST_MESH_VIEW_HANDLE(pMeshViewHandle);
	MeshHandle*			 pMeshHandle	= CAST_MESH_HANDLE(pMeshVoidHandle);
	OpenGLContextHandle* pContextHandle = CAST_CONTEXT_HANDLE(pRenderContext);

	NTT_UNUSED(pContextHandle);

	GL_ASSERT(glGenVertexArrays(1, &pHandle->vao));
	GL_ASSERT(glBindVertexArray(pHandle->vao));
	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, pMeshHandle->vbo));

	GL_ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));
	GL_ASSERT(glEnableVertexAttribArray(0));
	GL_ASSERT(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)));
	GL_ASSERT(glEnableVertexAttribArray(1));
	GL_ASSERT(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
	GL_ASSERT(glEnableVertexAttribArray(2));

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindVertexArray(0));

#if NTT_DEBUG
	GL_ASSERT(glGenVertexArrays(1, &pHandle->debugVAO));
	GL_ASSERT(glBindVertexArray(pHandle->debugVAO));
	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, pMeshHandle->debugVBO));

	GL_ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));
	GL_ASSERT(glEnableVertexAttribArray(0));

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindVertexArray(0));
#endif // NTT_DEBUG

	return RESULT_SUCCESS;
}

Result OpenGLMeshViewStorage::RemoveMeshViewImpl(const Pointer<void>& pMeshViewHandle)
{
	MeshViewHandle* pHandle = CAST_MESH_VIEW_HANDLE(pMeshViewHandle);
	GL_ASSERT(glDeleteVertexArrays(1, &pHandle->vao));

#if NTT_DEBUG
	GL_ASSERT(glDeleteVertexArrays(1, &pHandle->debugVAO));
#endif // NTT_DEBUG

	return RESULT_SUCCESS;
}

Result OpenGLMeshViewStorage::DrawMeshViewImpl(const Pointer<void>& pMeshViewHandle,
											   const Pointer<void>& pMeshVoidHandle,
											   const Pointer<void>& pRenderContext)
{
	MeshViewHandle*		 pHandle		= CAST_MESH_VIEW_HANDLE(pMeshViewHandle);
	MeshHandle*			 pMeshHandle	= CAST_MESH_HANDLE(pMeshVoidHandle);
	OpenGLContextHandle* pContextHandle = CAST_CONTEXT_HANDLE(pRenderContext);

	NTT_UNUSED(pMeshHandle);
	NTT_UNUSED(pContextHandle);

	GL_ASSERT(glBindVertexArray(pHandle->vao));
	GL_ASSERT(glDrawArrays(GL_TRIANGLES, 0, pMeshHandle->verticesCount));
	GL_ASSERT(glBindVertexArray(0));

	return RESULT_SUCCESS;
}

Result OpenGLMeshViewStorage::DrawDebugLineImpl(const Pointer<void>& pMeshViewHandle,
												const Pointer<void>& pMeshVoidHandle,
												const Pointer<void>& pRenderContext,
												f32					 lineWidth)
{
	NTT_UNUSED(pRenderContext);

	MeshViewHandle* pHandle		= CAST_MESH_VIEW_HANDLE(pMeshViewHandle);
	MeshHandle*		pMeshHandle = CAST_MESH_HANDLE(pMeshVoidHandle);

	GL_ASSERT(glLineWidth(lineWidth));
	GL_ASSERT(glBindVertexArray(pHandle->debugVAO));
	GL_ASSERT(glDrawArrays(GL_LINES, 0, pMeshHandle->verticesCount * 2));
	GL_ASSERT(glBindVertexArray(0));

	return RESULT_SUCCESS;
}

u32 OpenGLMeshViewStorage::GetMeshViewHandleSize() const
{
	return (u32)sizeof(MeshViewHandle);
}

} // namespace ntt
