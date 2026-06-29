#pragma once

#include "mesh_storage.h"
#include "shader_storage.h"

namespace ntt {

struct RenderGlobals
{
	Scope<MeshStorage>	 pMeshStorage;
	Scope<ShaderStorage> pShaderStorage;

	Result Shutdown()
	{
		pShaderStorage.Reset();
		pMeshStorage.Reset();
		return RESULT_SUCCESS;
	}
};

extern RenderGlobals g_RenderGlobals;

} // namespace ntt
