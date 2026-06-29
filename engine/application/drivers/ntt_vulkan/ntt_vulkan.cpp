#include "ntt_vulkan.h"
#include "ntt_vulkan_mesh_storage.h"
#include "ntt_vulkan_shader_storage.h"
#include "systems/render/render_globals.h"

#include "ntt_vulkan_inc.h"

namespace ntt {

struct VulkanContextHandle
{
	GLFWwindow* pWindow;
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

// clang-format off
static const char* s_InstanceExtensions[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
};

static const char* s_InstanceLayers[] = {
#if NTT_DEBUG
	"VK_LAYER_KHRONOS_validation",
#endif // NTT_DEBUG
};
// clang-format on

static VkInstance g_Instance = VK_NULL_HANDLE;

static bool checkInstanceAllExtensionsSupport();
static bool checkInstanceAllLayersSupport();

static Result createInstance();
static Result destroyInstance();

#if NTT_DEBUG
static Result setupDebugMessenger();
static Result destroyDebugMessenger();
#endif // NTT_DEBUG

static Result VulkanDriver_Initialize()
{
	if (!checkInstanceAllExtensionsSupport())
	{
		NTT_VULKAN_ERROR("Vulkan instance does not support all required extensions.");
		return RESULT_MISSING_VULKAN_EXTENSION;
	}

	if (!checkInstanceAllLayersSupport())
	{
		NTT_VULKAN_ERROR("Vulkan instance does not support all required layers.");
		return RESULT_MISSING_VULKAN_LAYER;
	}

	NTT_ASSERT_RESULT_SUCCESS(createInstance());

#if NTT_DEBUG
	NTT_ASSERT_RESULT_SUCCESS(setupDebugMessenger());
#endif // NTT_DEBUG

	NTT_VULKAN_INFO("Vulkan driver initialized.");

	return RESULT_SUCCESS;
}

static Result VulkanDriver_Shutdown()
{
#if NTT_DEBUG
	NTT_ASSERT_RESULT_SUCCESS(destroyDebugMessenger());
#endif // NTT_DEBUG

	NTT_ASSERT_RESULT_SUCCESS(destroyInstance());

	NTT_VULKAN_INFO("Vulkan driver shut down.");

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

static bool checkInstanceExtensionSupport(const char* extensionName);
static bool checkInstanceAllExtensionsSupport()
{
	for (const char* extension : s_InstanceExtensions)
	{
		if (!checkInstanceExtensionSupport(extension))
		{
			NTT_VULKAN_ERROR("Missing required Vulkan extension: %s", extension);
			return false;
		}
	}

	return true;
}

static bool checkInstanceExtensionSupport(const char* extensionName)
{
	u32 extensionCount = 0;
	VK_ASSERT_B(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

	VkExtensionProperties* pExtensions = new VkExtensionProperties[extensionCount];
	VK_ASSERT_B(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, pExtensions));

	bool found = false;
	for (u32 i = 0; i < extensionCount; ++i)
	{
		if (StringView(pExtensions[i].extensionName) == StringView(extensionName))
		{
			found = true;
			break;
		}
	}

	delete[] pExtensions;

	return found;
}

static bool checkInstanceLayerSupport(const char* layerName);
static bool checkInstanceAllLayersSupport()
{
	for (const char* layer : s_InstanceLayers)
	{
		if (!checkInstanceLayerSupport(layer))
		{
			NTT_VULKAN_ERROR("Missing required Vulkan layer: %s", layer);
			return false;
		}
	}

	return true;
}

static bool checkInstanceLayerSupport(const char* layerName)
{
	u32 layerCount = 0;
	VK_ASSERT_B(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

	VkLayerProperties* pLayers = new VkLayerProperties[layerCount];
	VK_ASSERT_B(vkEnumerateInstanceLayerProperties(&layerCount, pLayers));

	bool found = false;
	for (u32 i = 0; i < layerCount; ++i)
	{
		if (StringView(pLayers[i].layerName) == StringView(layerName))
		{
			found = true;
			break;
		}
	}

	delete[] pLayers;

	return found;
}

static Result createInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType			   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext			   = nullptr;
	appInfo.pApplicationName   = "NTT Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName		   = "NTT Engine";
	appInfo.engineVersion	   = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo createInfo{};
	createInfo.sType			= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext			= nullptr;
	createInfo.pApplicationInfo = &appInfo;

	u32 extensionCount				   = sizeof(s_InstanceExtensions) / sizeof(s_InstanceExtensions[0]);
	createInfo.enabledExtensionCount   = extensionCount;
	createInfo.ppEnabledExtensionNames = s_InstanceExtensions;

	u32 layerCount				   = sizeof(s_InstanceLayers) / sizeof(s_InstanceLayers[0]);
	createInfo.enabledLayerCount   = layerCount;
	createInfo.ppEnabledLayerNames = s_InstanceLayers;

	VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &g_Instance));

	NTT_VULKAN_DEBUG("Vulkan instance created.");

	return RESULT_SUCCESS;
}

static Result destroyInstance()
{
	if (g_Instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(g_Instance, nullptr);
		g_Instance = VK_NULL_HANDLE;
		NTT_VULKAN_DEBUG("Vulkan instance destroyed.");
	}

	return RESULT_SUCCESS;
}

#if NTT_DEBUG
static Result setupDebugMessenger()
{
	return RESULT_SUCCESS;
}

static Result destroyDebugMessenger()
{
	return RESULT_SUCCESS;
}
#endif // NTT_DEBUG

} // namespace ntt
