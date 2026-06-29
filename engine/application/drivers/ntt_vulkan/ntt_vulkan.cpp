#include "ntt_vulkan.h"
#include "ntt_vulkan_mesh_storage.h"
#include "ntt_vulkan_shader_storage.h"
#include "systems/render/render_globals.h"

namespace ntt {

struct VulkanContextHandle
{
	void* pWindow;
};

static Result VulkanDriver_Initialize();
static Result VulkanDriver_Shutdown();
static Result VulkanDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle);
static Result VulkanDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle);
static Result VulkanDriver_StartRender(Pointer<void> pDriverHandle);
static Result VulkanDriver_EndRender(Pointer<void> pDriverHandle);
static Result VulkanDriver_Present(Pointer<void> pDriverHandle);
static u32	  VulkanDriver_GetRenderContextHandleSize();

Result RegisterVulkanDriver()
{
	g_RenderDriver.Initialize				  = VulkanDriver_Initialize;
	g_RenderDriver.Shutdown					  = VulkanDriver_Shutdown;
	g_RenderDriver.CreateRenderContext		  = VulkanDriver_CreateRenderContext;
	g_RenderDriver.DestroyRenderContext		  = VulkanDriver_DestroyRenderContext;
	g_RenderDriver.StartRender				  = VulkanDriver_StartRender;
	g_RenderDriver.EndRender				  = VulkanDriver_EndRender;
	g_RenderDriver.Present					  = VulkanDriver_Present;
	g_RenderDriver.GetRenderContextHandleSize = VulkanDriver_GetRenderContextHandleSize;

	return RESULT_SUCCESS;
}

Result RegisterVulkanRenderer()
{
	g_RenderGlobals.pMeshStorage   = MakeScope<VulkanMeshStorage>(g_GlobalAllocators.pMalloc);
	g_RenderGlobals.pShaderStorage = MakeScope<VulkanShaderStorage>(g_GlobalAllocators.pMalloc);

	return RESULT_SUCCESS;
}

static Result VulkanDriver_Initialize()
{
	return RESULT_SUCCESS;
}

static Result VulkanDriver_Shutdown()
{
	return RESULT_SUCCESS;
}

static Result VulkanDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle)
{
	NTT_UNUSED(pWindowHandle);
	NTT_UNUSED(pRenderContextHandle);
	return RESULT_SUCCESS;
}

static Result VulkanDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle)
{
	NTT_UNUSED(pRenderContextHandle);
	return RESULT_SUCCESS;
}

static Result VulkanDriver_StartRender(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	return RESULT_SUCCESS;
}

static Result VulkanDriver_EndRender(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	return RESULT_SUCCESS;
}

static Result VulkanDriver_Present(Pointer<void> pDriverHandle)
{
	NTT_UNUSED(pDriverHandle);
	return RESULT_SUCCESS;
}

static u32 VulkanDriver_GetRenderContextHandleSize()
{
	return sizeof(VulkanContextHandle);
}

} // namespace ntt
