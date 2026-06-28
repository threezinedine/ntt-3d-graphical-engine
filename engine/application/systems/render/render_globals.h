#pragma once

#include "mesh_storage.h"

namespace ntt {

struct RenderGlobals
{
	Scope<MeshStorage> pMeshStorage;

	Result Shutdown()
	{
		pMeshStorage.Reset();
		return RESULT_SUCCESS;
	}
};

extern RenderGlobals g_RenderGlobals;

} // namespace ntt
