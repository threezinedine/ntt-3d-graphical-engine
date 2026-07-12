#pragma once

#include "mesh_storage.h"
#include "mesh_view_storage.h"
#include "shader_storage.h"
#include "texture_storage.h"

namespace ntt {

struct RenderGlobals
{
	Scope<MeshStorage>	   pMeshStorage;
	Scope<ShaderStorage>   pShaderStorage;
	Scope<TextureStorage>  pTextureStorage;
	Scope<MeshViewStorage> pMeshViewStorage;

	Result Shutdown()
	{
		pMeshViewStorage.Reset();
		pTextureStorage.Reset();
		pShaderStorage.Reset();
		pMeshStorage.Reset();
		return RESULT_SUCCESS;
	}
};

extern RenderGlobals g_RenderGlobals;

#define NTT_MESH_STORAGE	  (g_RenderGlobals.pMeshStorage)
#define NTT_SHADER_STORAGE	  (g_RenderGlobals.pShaderStorage)
#define NTT_TEXTURE_STORAGE	  (g_RenderGlobals.pTextureStorage)
#define NTT_MESH_VIEW_STORAGE (g_RenderGlobals.pMeshViewStorage)

} // namespace ntt
