#include "ntt_opengl_mesh_storage.h"
#include "ntt_opengl_inc.h"

namespace ntt {

struct MeshHandle
{
	u32 vao;
	u32 vbo;
};

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

Result OpenGLMeshStorage::AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle)
{
	u32 verticesCount = mesh.vertices.GetCount();
	u32 sizeInBytes	  = verticesCount * sizeof(Vertex);
	u8* pVertexData	  = reinterpret_cast<u8*>(&mesh.vertices[0]);

	MeshHandle* pHandle = reinterpret_cast<MeshHandle*>(pMeshHandle.Get());

	if (pHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GL_ASSERT(glGenVertexArrays(1, &pHandle->vao));
	GL_ASSERT(glGenBuffers(1, &pHandle->vbo));

	GL_ASSERT(glBindVertexArray(pHandle->vao));
	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, pHandle->vbo));

	GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, sizeInBytes, pVertexData, GL_STATIC_DRAW));

	GL_ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));
	GL_ASSERT(glEnableVertexAttribArray(0));
	GL_ASSERT(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)));
	GL_ASSERT(glEnableVertexAttribArray(1));
	GL_ASSERT(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)));
	GL_ASSERT(glEnableVertexAttribArray(2));

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindVertexArray(0));

	return RESULT_SUCCESS;
}

Result OpenGLMeshStorage::DrawMeshImpl(const Pointer<void>& pMeshHandle)
{
	MeshHandle* pHandle = reinterpret_cast<MeshHandle*>(pMeshHandle.Get());

	if (pHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GL_ASSERT(glBindVertexArray(pHandle->vao));
	GL_ASSERT(glDrawArrays(GL_TRIANGLES, 0, 3)); // Assuming the mesh is made of triangles
	GL_ASSERT(glBindVertexArray(0));

	return RESULT_SUCCESS;
}

Result OpenGLMeshStorage::RemoveMeshImpl(const Pointer<void>& pMeshHandle)
{
	MeshHandle* pHandle = reinterpret_cast<MeshHandle*>(pMeshHandle.Get());

	if (pHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	GL_ASSERT(glDeleteBuffers(1, &pHandle->vbo));
	GL_ASSERT(glDeleteVertexArrays(1, &pHandle->vao));

	return RESULT_SUCCESS;
}

u32 OpenGLMeshStorage::GetMeshHandleSize() const
{
	return (u32)sizeof(MeshHandle);
}

} // namespace ntt
