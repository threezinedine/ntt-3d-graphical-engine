#pragma once

#include "mesh_storage.h"
#include "shader_storage.h"
#include "texture_storage.h"

namespace ntt {

struct RenderGlobals
{
	Scope<MeshStorage>	  pMeshStorage;
	Scope<ShaderStorage>  pShaderStorage;
	Scope<TextureStorage> pTextureStorage;

	Result Shutdown()
	{
		pTextureStorage.Reset();
		pShaderStorage.Reset();
		pMeshStorage.Reset();
		return RESULT_SUCCESS;
	}
};

extern RenderGlobals g_RenderGlobals;

#define NTT_MESH_STORAGE	(g_RenderGlobals.pMeshStorage)
#define NTT_SHADER_STORAGE	(g_RenderGlobals.pShaderStorage)
#define NTT_TEXTURE_STORAGE (g_RenderGlobals.pTextureStorage)

} // namespace ntt
