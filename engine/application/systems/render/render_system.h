#pragma once

#include "systems/display/display_system.h"
#include "systems/system.h"

namespace ntt {

typedef u32 ShaderID;
typedef u32 RenderContextID;

constexpr RenderContextID INVALID_RENDER_CONTEXT_ID = static_cast<u32>(-1);

class RenderSystem : public System
{
	NTT_OBJECT_DERIVED_DECLARE(RenderSystem, System)

public:
	RenderSystem();
	virtual ~RenderSystem();

public:
	RenderContextID CreateRenderContext(WindowID windowID);
	Result			DestroyRenderContext(RenderContextID renderContextID);

public:
	Result BeginRender(RenderContextID renderContextID);
	Result EndRender(RenderContextID renderContextID);
	Result Present(RenderContextID renderContextID);

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

public:
	struct RenderContext
	{
		Pointer<void> pRenderContextHandle;
		ShaderID	  defaultMeshShaderID;
#if NTT_DEBUG
		ShaderID defaultDebugLineShaderID;
#endif // NTT_DEBUG
	};

	friend class ShaderStorage;
	friend class MeshStorage;

private:
	Scope<Storage<RenderContext>> m_pRenderContextStorage;
};

} // namespace ntt
