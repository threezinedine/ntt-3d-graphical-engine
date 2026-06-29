#include "render_system.h"
#include "drivers/ntt_opengl/ntt_opengl.h"
#include "drivers/ntt_vulkan/ntt_vulkan.h"
#include "render_driver.h"
#include "render_globals.h"
#include "systems/display/display_driver.h"
#include "systems/system_globals.h"

namespace ntt {

NTT_OBJECT_DERIVED_DEFINE(RenderSystem, System)

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

Result RenderSystem::InitializeImpl()
{
	m_pRenderContextStorage = MakeScope<Storage<RenderContext>>(g_GlobalAllocators.pMalloc);

	if (NTT_ARG_BOOL(USE_VULKAN))
	{
		NTT_ASSERT_RESULT_SUCCESS(RegisterVulkanDriver());
	}
	else
	{
		NTT_ASSERT_RESULT_SUCCESS(RegisterOpenGLDriver());
	}

	NTT_ASSERT_RESULT_SUCCESS(g_RenderDriver.Initialize());

	if (NTT_ARG_BOOL(USE_VULKAN))
	{
		NTT_ASSERT_RESULT_SUCCESS(RegisterVulkanRenderer());
	}
	else
	{
		NTT_ASSERT_RESULT_SUCCESS(RegisterOpenGLRenderer());
	}

	if (g_RenderGlobals.pMeshStorage == nullptr)
	{
		NTT_RENDER_ERROR("Failed to initialize mesh storage.");
		return RESULT_REGISTER_OPENGL_RENDERER_FAILED;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pMeshStorage->Initialize());
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->Initialize());

	return RESULT_SUCCESS;
}

Result RenderSystem::ShutdownImpl()
{
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pMeshStorage->Shutdown());

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(g_RenderDriver.Shutdown());

	m_pRenderContextStorage.Reset();

	return RESULT_SUCCESS;
}

RenderContextID RenderSystem::CreateRenderContext(WindowID windowID)
{
	if (windowID == INVALID_WINDOW_ID)
	{
		NTT_RENDER_ERROR("Invalid window ID. Cannot create render context.");
		return INVALID_RENDER_CONTEXT_ID;
	}

	if (SystemGlobals::pDisplaySystem == nullptr)
	{
		NTT_RENDER_ERROR("Display system is not initialized. Cannot create render context.");
		return INVALID_RENDER_CONTEXT_ID;
	}

	DisplaySystem::WindowInfo* pWindowInfo = SystemGlobals::pDisplaySystem->m_pWindowIDStorage->Get(windowID);

	RenderContextID id			   = m_pRenderContextStorage->Add(RenderContext{});
	RenderContext*	pRenderContext = m_pRenderContextStorage->Get(id);
	pRenderContext->pRenderContextHandle =
		ALLOCATOR_SAFE(g_GlobalAllocators.pMalloc)->Allocate(g_RenderDriver.GetRenderContextHandleSize());

	if (pRenderContext == nullptr)
	{
		NTT_RENDER_ERROR("Failed to create render context. Invalid render context handle.");
		return INVALID_RENDER_CONTEXT_ID;
	}

	NTT_ASSERT_RESULT_SUCCESS(
		g_RenderDriver.CreateRenderContext(pWindowInfo->pWindowHandle, pRenderContext->pRenderContextHandle));

	return id;
}

Result RenderSystem::DestroyRenderContext(RenderContextID renderContextID)
{
	if (!m_pRenderContextStorage->IsActive(renderContextID))
	{
		return RESULT_INACTIVE_STORAGE_INDEX;
	}

	RenderContext* pRenderContext = m_pRenderContextStorage->Get(renderContextID);

	if (pRenderContext == nullptr || pRenderContext->pRenderContextHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_RenderDriver.DestroyRenderContext(pRenderContext->pRenderContextHandle));
	NTT_ASSERT_RESULT_SUCCESS(pRenderContext->pRenderContextHandle.Free());
	NTT_ASSERT_RESULT_SUCCESS(m_pRenderContextStorage->Remove(renderContextID));

	return RESULT_SUCCESS;
}

Result RenderSystem::BeginRender(RenderContextID renderContextID)
{
	RenderContext* pRenderContext = m_pRenderContextStorage->Get(renderContextID);

	if (pRenderContext == nullptr || pRenderContext->pRenderContextHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	return g_RenderDriver.StartRender(pRenderContext->pRenderContextHandle);
}

Result RenderSystem::EndRender(RenderContextID renderContextID)
{
	RenderContext* pRenderContext = m_pRenderContextStorage->Get(renderContextID);

	if (pRenderContext == nullptr || pRenderContext->pRenderContextHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	return g_RenderDriver.EndRender(pRenderContext->pRenderContextHandle);
}

Result RenderSystem::Present(RenderContextID renderContextID)
{
	RenderContext* pRenderContext = m_pRenderContextStorage->Get(renderContextID);

	if (pRenderContext == nullptr || pRenderContext->pRenderContextHandle == nullptr)
	{
		return RESULT_NULL_POINTER;
	}

	return g_RenderDriver.Present(pRenderContext->pRenderContextHandle);
}

} // namespace ntt
