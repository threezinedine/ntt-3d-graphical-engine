#pragma once

#include "vertex.h"

namespace ntt {

#if 0
enum MeshTopology
{
	MESH_TOPOLOGY_TRIANGLES,
	MESH_TOPOLOGY_LINES,
	MESH_TOPOLOGY_POINTS
};

// Vertices array type for ABCBCD (1 rect) or ABCD (1 rect), i mean how the vertices array is structured
enum MeshVertexType
{
	MESH_VERTEX_TYPE_TRIANGLES, // ABCBCD (1 rect)
	MESH_VERTEX_TYPE_QUADS		// ABCD (1 rect)
};
#endif

struct Mesh
{
	Array<Vertex> vertices;

	Mesh() = default;

	Mesh(Mesh&& other) noexcept
		: vertices(static_cast<Array<Vertex>&&>(other.vertices))
	{
	}

	Mesh& operator=(Mesh&& other) noexcept
	{
		vertices = static_cast<Array<Vertex>&&>(other.vertices);
		return *this;
	}
};

} // namespace ntt
