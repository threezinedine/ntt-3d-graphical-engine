#include "ntt_opengl_mesh_storage.h"
#include "ntt_opengl_inc.h"

namespace ntt {

OpenGLMeshStorage::OpenGLMeshStorage(IAllocator* pAllocator)
	: MeshStorage(pAllocator)
{
}

OpenGLMeshStorage::~OpenGLMeshStorage()
{
}

Result OpenGLMeshStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result OpenGLMeshStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

Result OpenGLMeshStorage::AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle, bool dynamic)
{
	u32 verticesCount = mesh.vertices.GetCount();
	u32 sizeInBytes	  = verticesCount * sizeof(Vertex);
	u8* pVertexData	  = reinterpret_cast<u8*>(&mesh.vertices[0]);

	MeshHandle* pHandle	   = CAST_MESH_HANDLE(pMeshHandle);
	pHandle->verticesCount = verticesCount;

	GL_ASSERT(glGenBuffers(1, &pHandle->vbo));
	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, pHandle->vbo));

	GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, sizeInBytes, pVertexData, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindVertexArray(0));

#if NTT_DEBUG
	GL_ASSERT(glGenBuffers(1, &pHandle->debugVBO));

	u32 debugVerticesCount = verticesCount * 2; // Each edge will have two vertices
	u32 debugSizeInBytes   = debugVerticesCount * sizeof(Vertex);

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, pHandle->debugVBO));
	GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, debugSizeInBytes, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	for (u32 i = 0; i < verticesCount; ++i)
	{
		Vertex& vertex	   = mesh.vertices[i];
		Vertex& nextVertex = mesh.vertices[(i + 1) % verticesCount];
		GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, i * 2 * sizeof(Vertex), sizeof(Vertex), &vertex));
		GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, (i * 2 + 1) * sizeof(Vertex), sizeof(Vertex), &nextVertex));
	}

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
#endif // NTT_DEBUG

	return RESULT_SUCCESS;
}

Result OpenGLMeshStorage::RemoveMeshImpl(const Pointer<void>& pMeshHandle)
{
	MeshHandle* pHandle = CAST_MESH_HANDLE(pMeshHandle);

	GL_ASSERT(glDeleteBuffers(1, &pHandle->vbo));

#if NTT_DEBUG
	GL_ASSERT(glDeleteBuffers(1, &pHandle->debugVBO));
#endif // NTT_DEBUG

	return RESULT_SUCCESS;
}

u32 OpenGLMeshStorage::GetMeshHandleSize() const
{
	return (u32)sizeof(MeshHandle);
}

} // namespace ntt
