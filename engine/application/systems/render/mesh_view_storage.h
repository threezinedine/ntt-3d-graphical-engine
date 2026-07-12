#pragma once

#include "render_system_types.h"
#include "services.h"

namespace ntt {

typedef u32 MeshID;
typedef u32 RenderContextID;
typedef u32 ShaderID;
typedef u32 TextureID;

class MeshViewStorage
{
public:
	MeshViewStorage(IAllocator* pAllocator = nullptr);
	~MeshViewStorage();

	Result Initialize();
	Result Shutdown();

	MeshViewID AddMeshView(MeshID meshID, RenderContextID renderContextID);
	Result	   RemoveMeshView(MeshViewID meshViewID);

	Result DrawMeshView(MeshViewID meshViewID);
#if NTT_DEBUG
	Result DrawDebugLine(MeshViewID meshViewID);
#endif // NTT_DEBUG
	Result SetShader(MeshViewID meshViewID, ShaderID shaderID);
#if NTT_DEBUG
	Result SetDebugLineShader(MeshViewID meshViewID, ShaderID shaderID);
	Result SetDebugLineWidth(MeshViewID meshViewID, f32 lineWidth);
#endif // NTT_DEBUG

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
public:                                                                                                                \
	Result SetUniform##typeName(MeshViewID meshViewID, const char* pUniformName, type value);
#define UNIFORM_TYPE_SAMPLER_DEF(type, typeName, uppercase, glType) UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF

#if NTT_DEBUG
#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
public:                                                                                                                \
	Result SetDebugLineUniform##typeName(MeshViewID meshViewID, const char* pUniformName, type value);
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF
#endif // NTT_DEBUG

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddMeshViewImpl(const Pointer<void>& pMeshViewHandle,
								   const Pointer<void>& pMeshHandle,
								   const Pointer<void>& pRenderContext)		= 0;
	virtual Result RemoveMeshViewImpl(const Pointer<void>& pMeshViewHandle) = 0;

	virtual Result DrawMeshViewImpl(const Pointer<void>& pMeshViewHanle,
									const Pointer<void>& pMeshHandle,
									const Pointer<void>& pRenderContext) = 0;
	virtual Result DrawDebugLineImpl(const Pointer<void>& pMeshViewHandle,
									 const Pointer<void>& pMeshHandle,
									 const Pointer<void>& pRenderContext,
									 f32				  lineWidth)						 = 0;

	virtual u32 GetMeshViewHandleSize() const = 0;

public:
	struct MeshViewNode
	{
		MeshID			meshID;
		RenderContextID renderContextID;
		ShaderID		shaderID;
#if NTT_DEBUG
		ShaderID debugLineShaderID;
		f32		 lineWidth;
#endif // NTT_DEBUG
		Pointer<void> pMeshViewHandle;
	};

private:
	IAllocator*					 m_pAllocator;
	Scope<Storage<MeshViewNode>> m_pMeshViewStorage;
};

} // namespace ntt
