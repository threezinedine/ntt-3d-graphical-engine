#pragma once
#include "services.h"

namespace ntt {

typedef u32		 MeshViewID;
const MeshViewID INVALID_MESH_VIEW_ID = static_cast<u32>(-1);

typedef u32			TextureID;
constexpr TextureID INVALID_TEXTURE_ID = static_cast<u32>(-1);

typedef u32	 MeshID;
const MeshID INVALID_MESH_ID = static_cast<u32>(-1);

typedef u32		   ShaderID;
constexpr ShaderID INVALID_SHADER_ID = static_cast<u32>(-1);

extern ShaderID defaultMeshShaderID;
#if NTT_DEBUG
extern ShaderID defaultDebugLineShaderID;
#endif // NTT_DEBUG

} // namespace ntt
