#pragma once

#include "systems/display/display_system.h"
#include "systems/system.h"

namespace ntt {

typedef u32 ShaderID;
typedef u32 RenderContextID;
typedef u32 MeshViewID;
typedef u32 MeshID;

constexpr RenderContextID INVALID_RENDER_CONTEXT_ID = static_cast<u32>(-1);

class MeshViewStorage;

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
	};

	friend class ShaderStorage;
	friend class MeshStorage;
	friend class MeshViewStorage;

private:
	Scope<Storage<RenderContext>> m_pRenderContextStorage;
};

} // namespace ntt
