#if NTT_VULKAN

#include "ntt_vulkan.h"
#include "ntt_vulkan_mesh_storage.h"
#include "ntt_vulkan_shader_storage.h"
#include "systems/display/display_driver.h"
#include "systems/render/render_globals.h"

#include "ntt_vulkan_inc.h"

namespace ntt {

static constexpr u32 INVALID_QUEUE_FAMILY_INDEX = UINT32_MAX;

struct VulkanContextHandle
{
	GLFWwindow*	 pWindow;
	VkSurfaceKHR surface;
	u32			 graphicsQueueFamilyIndex;
	u32			 presentQueueFamilyIndex;
	u32			 transferQueueFamilyIndex;
	u32			 computeQueueFamilyIndex;
};

#define CAST(handle)                                                                                                   \
	reinterpret_cast<VulkanContextHandle*>(handle.Get());                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

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
#if NTT_DEBUG
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // NTT_DEBUG
};

static const char* s_InstanceLayers[] = {
#if NTT_DEBUG
	"VK_LAYER_KHRONOS_validation",
#endif // NTT_DEBUG
};

static const char* s_DeviceExtensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
// clang-format on

#if NTT_DEBUG
static PFN_vkCreateDebugUtilsMessengerEXT  s_pfnCreateDebugUtilsMessengerEXT  = nullptr;
static PFN_vkDestroyDebugUtilsMessengerEXT s_pfnDestroyDebugUtilsMessengerEXT = nullptr;
#endif // NTT_DEBUG

static bool	  checkInstanceAllExtensionsSupport();
static bool	  checkInstanceAllLayersSupport();
static Result loadVulkanInstanceMethods();

static VkInstance g_Instance = VK_NULL_HANDLE;
static Result	  createInstance();
static Result	  destroyInstance();

#if NTT_DEBUG
static Result					setupDebugMessenger();
static Result					destroyDebugMessenger();
static VkDebugUtilsMessengerEXT g_DebugMessenger = VK_NULL_HANDLE;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT				messageType,
													const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
													void*										pUserData);
#endif // NTT_DEBUG

static u32										 s_PhysicalDeviceCount = 0;
static Scope<Array<VkPhysicalDevice>>			 s_pPhysicalDevices;
static Scope<Array<VkPhysicalDeviceProperties*>> s_pPhysicalDeviceProperties;
static VkPhysicalDevice							 g_PhysicalDevice = VK_NULL_HANDLE;

static Result enumeratePhysicalDevices();
static Result destroyPhysicalDevices();
static Result choosePhysicalDevice();
static Result verifyDeviceExtensionsSupport(VkPhysicalDevice physicalDevice);

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

	NTT_ASSERT_RESULT_SUCCESS(loadVulkanInstanceMethods());

#if NTT_DEBUG
	NTT_ASSERT_RESULT_SUCCESS(setupDebugMessenger());
#endif // NTT_DEBUG

	NTT_ASSERT_RESULT_SUCCESS(enumeratePhysicalDevices());
	NTT_ASSERT_RESULT_SUCCESS(choosePhysicalDevice());
	NTT_ASSERT_RESULT_SUCCESS(verifyDeviceExtensionsSupport(g_PhysicalDevice));

	NTT_VULKAN_INFO("Vulkan driver initialized.");

	return RESULT_SUCCESS;
}

static Result VulkanDriver_Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(destroyPhysicalDevices());

#if NTT_DEBUG
	NTT_ASSERT_RESULT_SUCCESS(destroyDebugMessenger());
#endif // NTT_DEBUG

	NTT_ASSERT_RESULT_SUCCESS(destroyInstance());

	NTT_VULKAN_INFO("Vulkan driver shut down.");

	return RESULT_SUCCESS;
}

static Result chooseQueueFamilies(VulkanContextHandle* pContextHandle, VkSurfaceKHR surface);

static Result VulkanDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle)
{
#if NTT_GLFW
	GLFWwindow*			 pWindow		= (GLFWwindow*)g_DisplayDriver.GetWindowHandle(pWindowHandle);
	VulkanContextHandle* pContextHandle = CAST(pRenderContextHandle);

	pContextHandle->pWindow					 = pWindow;
	pContextHandle->surface					 = VK_NULL_HANDLE;
	pContextHandle->graphicsQueueFamilyIndex = INVALID_QUEUE_FAMILY_INDEX;
	pContextHandle->presentQueueFamilyIndex	 = INVALID_QUEUE_FAMILY_INDEX;
	pContextHandle->transferQueueFamilyIndex = INVALID_QUEUE_FAMILY_INDEX;
	pContextHandle->computeQueueFamilyIndex	 = INVALID_QUEUE_FAMILY_INDEX;

	VK_ASSERT(glfwCreateWindowSurface(g_Instance, pWindow, nullptr, &pContextHandle->surface));
	NTT_ASSERT_RESULT_SUCCESS(chooseQueueFamilies(pContextHandle, pContextHandle->surface));
#else  // NTT_GLFW
	NTT_UNUSED(pWindowHandle);
	NTT_UNUSED(pRenderContextHandle);
#endif // NTT_GLFW

	return RESULT_SUCCESS;
}

static Result VulkanDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle)
{
	VulkanContextHandle* pContextHandle = CAST(pRenderContextHandle);

	vkDestroySurfaceKHR(g_Instance, pContextHandle->surface, nullptr);

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

static Result loadVulkanInstanceMethods()
{
#define LOAD_INSTANCE_METHOD(name)                                                                                     \
	s_pfn##name = reinterpret_cast<PFN_vk##name>(vkGetInstanceProcAddr(g_Instance, "vk" #name));                       \
	if (s_pfn##name == nullptr)                                                                                        \
	{                                                                                                                  \
		NTT_VULKAN_ERROR("Failed to load Vulkan instance method: %s", "vk" #name);                                     \
		return RESULT_VULKAN_ERROR;                                                                                    \
	}

#if NTT_DEBUG
	LOAD_INSTANCE_METHOD(CreateDebugUtilsMessengerEXT);
	LOAD_INSTANCE_METHOD(DestroyDebugUtilsMessengerEXT);
#endif // NTT_DEBUG

	return RESULT_SUCCESS;
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

	u32 extensionCount = sizeof(s_InstanceExtensions) / sizeof(s_InstanceExtensions[0]);

#if NTT_GLFW
	u32				   glfwExtensionCount = 0;
	const char**	   glfwExtensions	  = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	Array<const char*> requiredExtensions(glfwExtensionCount + extensionCount, g_GlobalAllocators.pStack);
	MemCopy(&requiredExtensions[0], glfwExtensions, glfwExtensionCount * sizeof(const char*));
	MemCopy(&requiredExtensions[glfwExtensionCount], s_InstanceExtensions, extensionCount * sizeof(const char*));
	createInfo.enabledExtensionCount   = glfwExtensionCount + extensionCount;
	createInfo.ppEnabledExtensionNames = &requiredExtensions[0];

#else  // NTT_GLFW
	createInfo.enabledExtensionCount   = extensionCount;
	createInfo.ppEnabledExtensionNames = s_InstanceExtensions;
#endif // NTT_GLFW

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
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType		   = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData	   = nullptr;

	VK_ASSERT(s_pfnCreateDebugUtilsMessengerEXT(g_Instance, &createInfo, nullptr, &g_DebugMessenger));

	NTT_VULKAN_DEBUG("Vulkan debug messenger created.");

	return RESULT_SUCCESS;
}

static Result destroyDebugMessenger()
{
	s_pfnDestroyDebugUtilsMessengerEXT(g_Instance, g_DebugMessenger, nullptr);

	NTT_VULKAN_DEBUG("Vulkan debug messenger destroyed.");

	return RESULT_SUCCESS;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT				messageType,
													const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
													void*										pUserData)
{
	NTT_UNUSED(messageType);
	NTT_UNUSED(pUserData);

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		NTT_VULKAN_ERROR("Vulkan validation layer: %s", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		NTT_VULKAN_WARN("Vulkan validation layer: %s", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		NTT_VULKAN_INFO("Vulkan validation layer: %s", pCallbackData->pMessage);
	}

	return VK_FALSE;
}
#endif // NTT_DEBUG

static Result enumeratePhysicalDevices()
{
	s_pPhysicalDevices			= MakeScope<Array<VkPhysicalDevice>>(g_GlobalAllocators.pMalloc);
	s_pPhysicalDeviceProperties = MakeScope<Array<VkPhysicalDeviceProperties*>>(g_GlobalAllocators.pMalloc);

	u32 deviceCount = 0;
	VK_ASSERT(vkEnumeratePhysicalDevices(g_Instance, &deviceCount, nullptr));

	s_pPhysicalDevices->Resize(deviceCount);
	s_pPhysicalDeviceProperties->Resize(deviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(g_Instance, &deviceCount, &(*s_pPhysicalDevices.Get())[0]));
	s_PhysicalDeviceCount = deviceCount;

	NTT_VULKAN_INFO("Found %u Vulkan physical devices:", deviceCount);

	for (u32 i = 0; i < deviceCount; ++i)
	{
		VkPhysicalDeviceProperties* pProperties = new VkPhysicalDeviceProperties();
		vkGetPhysicalDeviceProperties((*s_pPhysicalDevices.Get())[i], pProperties);
		(*s_pPhysicalDeviceProperties.Get())[i] = pProperties;

		NTT_VULKAN_INFO("\tDevice %u: %s", i, pProperties->deviceName);
	}

	return RESULT_SUCCESS;
}

static Result destroyPhysicalDevices()
{
	s_pPhysicalDeviceProperties.Reset();
	s_pPhysicalDevices.Reset();
	s_PhysicalDeviceCount = 0;

	NTT_VULKAN_DEBUG("Vulkan physical devices destroyed.");

	return RESULT_SUCCESS;
}

static u32 ratePhysicalDeviceScore(u32 index);

static Result choosePhysicalDevice()
{
	u32 bestScore = 0;
	u32 bestIndex = 0;

	for (u32 i = 0; i < s_PhysicalDeviceCount; ++i)
	{
		u32 score = ratePhysicalDeviceScore(i);

		NTT_VULKAN_DEBUG("Physical device %s score: %u", (*s_pPhysicalDeviceProperties.Get())[i]->deviceName, score);

		if (score > bestScore)
		{
			bestScore = score;
			bestIndex = i;
		}
	}

	g_PhysicalDevice = (*s_pPhysicalDevices.Get())[bestIndex];

	NTT_VULKAN_INFO("Chosen Vulkan physical device: %s", (*s_pPhysicalDeviceProperties.Get())[bestIndex]->deviceName);

	return RESULT_SUCCESS;
}

static Result verifyDeviceExtensionsSupport(VkPhysicalDevice physicalDevice)
{
	u32 extensionCount = 0;
	VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));

	Array<VkExtensionProperties> availableExtensions(extensionCount, g_GlobalAllocators.pStack);
	VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, &availableExtensions[0]));

	for (const char* requiredExtension : s_DeviceExtensions)
	{
		bool found = false;
		for (u32 i = 0; i < extensionCount; ++i)
		{
			if (StringView(availableExtensions[i].extensionName) == StringView(requiredExtension))
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			NTT_VULKAN_ERROR("Missing required Vulkan device extension: %s", requiredExtension);
			return RESULT_MISSING_VULKAN_EXTENSION;
		}
	}

	return RESULT_SUCCESS;
}

static u32 ratePhysicalDeviceScore(u32 index)
{
	VkPhysicalDeviceProperties* pProperties = (*s_pPhysicalDeviceProperties.Get())[index];

	if (pProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		return 1000 + pProperties->limits.maxImageDimension2D;
	}
	else if (pProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		return 500 + pProperties->limits.maxImageDimension2D;
	}
	else
	{
		return 0;
	}
}

static Result chooseQueueFamilies(VulkanContextHandle* pContextHandle, VkSurfaceKHR surface)
{
	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queueFamilyCount, nullptr);

	if (queueFamilyCount == 0)
	{
		NTT_VULKAN_ERROR("No queue families found for the chosen physical device.");
		return RESULT_VULKAN_ERROR;
	}

	Array<VkQueueFamilyProperties> queueFamilies(queueFamilyCount, g_GlobalAllocators.pStack);
	vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queueFamilyCount, &queueFamilies[0]);

	bool graphicsQueueFound = false;
	bool presentQueueFound	= false;

	for (u32 i = 0; i < queueFamilyCount; ++i)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFound)
		{
			pContextHandle->graphicsQueueFamilyIndex = i;
			graphicsQueueFound						 = true;
		}

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, i, surface, &presentSupport);
		if (presentSupport && !presentQueueFound)
		{
			pContextHandle->presentQueueFamilyIndex = i;
			presentQueueFound						= true;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			pContextHandle->transferQueueFamilyIndex = i;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			pContextHandle->computeQueueFamilyIndex = i;
		}
	}

	if (pContextHandle->graphicsQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_ERROR("No graphics queue family found for the chosen physical device.");
		return RESULT_VULKAN_ERROR;
	}
	else
	{
		NTT_VULKAN_DEBUG("Graphics queue family index: %u", pContextHandle->graphicsQueueFamilyIndex);
	}

	if (pContextHandle->presentQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_ERROR("No present queue family found for the chosen physical device.");
		return RESULT_VULKAN_ERROR;
	}
	else
	{
		NTT_VULKAN_DEBUG("Present queue family index: %u", pContextHandle->presentQueueFamilyIndex);
	}

	if (pContextHandle->transferQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_WARN("No transfer queue family found for the chosen physical device.");
	}
	else
	{
		NTT_VULKAN_DEBUG("Transfer queue family index: %u", pContextHandle->transferQueueFamilyIndex);
	}

	if (pContextHandle->computeQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_WARN("No compute queue family found for the chosen physical device.");
	}
	else
	{
		NTT_VULKAN_DEBUG("Compute queue family index: %u", pContextHandle->computeQueueFamilyIndex);
	}

	return RESULT_SUCCESS;
}

} // namespace ntt

#endif // NTT_VULKAN