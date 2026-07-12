#if NTT_VULKAN

#include "ntt_vulkan.h"
#include "ntt_vulkan_mesh_storage.h"
#include "ntt_vulkan_mesh_view_storage.h"
#include "ntt_vulkan_shader_storage.h"
#include "ntt_vulkan_texture_storage.h"
#include "systems/display/display_driver.h"
#include "systems/render/render_globals.h"
#include <cstring>

#include "ntt_vulkan_inc.h"

namespace ntt {

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static constexpr u32 INVALID_QUEUE_FAMILY_INDEX = UINT32_MAX;

static Result VulkanDriver_Initialize();
static Result VulkanDriver_Shutdown();
static Result VulkanDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle);
static Result VulkanDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle);
static Result VulkanDriver_StartRender(Pointer<void> pDriverHandle);
static Result VulkanDriver_EndRender(Pointer<void> pDriverHandle);
static Result VulkanDriver_Present(Pointer<void> pDriverHandle);
static u32	  VulkanDriver_GetRenderContextHandleSize();

static Result recreateSwapchain(VulkanContextHandle* pContextHandle);
static Result VulkanDriver_OnWindowResize(u32 width, u32 height, void* pUserData);

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
	g_RenderGlobals.pMeshStorage	 = MakeScope<VulkanMeshStorage>(g_GlobalAllocators.pMalloc);
	g_RenderGlobals.pShaderStorage	 = MakeScope<VulkanShaderStorage>(g_GlobalAllocators.pMalloc);
	g_RenderGlobals.pTextureStorage	 = MakeScope<VulkanTextureStorage>(g_GlobalAllocators.pMalloc);
	g_RenderGlobals.pMeshViewStorage = MakeScope<VulkanMeshViewStorage>(g_GlobalAllocators.pMalloc);

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

static Result createInstance();
static Result destroyInstance();

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
VulkanGlobals									 g_VulkanGlobals = {};

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
	NTT_ASSERT_RESULT_SUCCESS(verifyDeviceExtensionsSupport(g_VulkanGlobals.physicalDevice));

	NTT_VULKAN_INFO("Vulkan driver initialized.");

	return RESULT_SUCCESS;
}

static Result chooseQueueFamilies(VkSurfaceKHR surface);
static Result createLogicalDevice();
static Result destroyLogicalDevice();
static Result createSwapchain(VulkanContextHandle* pContextHandle, Vec2u& pWindowSize);
static Result destroySwapchain(VulkanContextHandle* pContextHandle);
static Result createRenderPass();
static Result destroyRenderPass();
static Result createSwapchainFramebuffers(VulkanContextHandle* pContextHandle);
static Result destroySwapchainFramebuffers(VulkanContextHandle* pContextHandle);
static Result createCommandPools();
static Result destroyCommandPools();
static Result createCommandBuffers(VulkanContextHandle* pContextHandle);
static Result destroyCommandBuffers(VulkanContextHandle* pContextHandle);
static Result createSyncObjects(VulkanContextHandle* pContextHandle);
static Result destroySyncObjects(VulkanContextHandle* pContextHandle);

static Result VulkanDriver_Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(destroyRenderPass());
	NTT_ASSERT_RESULT_SUCCESS(destroyCommandPools());
	NTT_ASSERT_RESULT_SUCCESS(destroyLogicalDevice());
	NTT_ASSERT_RESULT_SUCCESS(destroyPhysicalDevices());

#if NTT_DEBUG
	NTT_ASSERT_RESULT_SUCCESS(destroyDebugMessenger());
#endif // NTT_DEBUG

	NTT_ASSERT_RESULT_SUCCESS(destroyInstance());

	NTT_VULKAN_INFO("Vulkan driver shut down.");

	return RESULT_SUCCESS;
}

static Result VulkanDriver_CreateRenderContext(Pointer<void> pWindowHandle, Pointer<void>& pRenderContextHandle)
{
#if NTT_GLFW
	GLFWwindow*			 pWindow		= (GLFWwindow*)g_DisplayDriver.GetWindowHandle(pWindowHandle);
	VulkanContextHandle* pContextHandle = VK_CONTEXT_CAST(pRenderContextHandle);
	Vec2u				 windowSize		= g_DisplayDriver.GetWindowSize(pWindowHandle);

	pContextHandle->pWindowHandle		   = pWindowHandle;
	pContextHandle->surface				   = VK_NULL_HANDLE;
	pContextHandle->swapchain			   = VK_NULL_HANDLE;
	pContextHandle->pSwapchainImages	   = MakeScope<Array<VkImage>>(g_GlobalAllocators.pMalloc, 1);
	pContextHandle->swapchainImageCount	   = 0;
	pContextHandle->pSwapchainImageViews   = MakeScope<Array<VkImageView>>(g_GlobalAllocators.pMalloc, 1);
	pContextHandle->pSwapchainFramebuffers = MakeScope<Array<VkFramebuffer>>(g_GlobalAllocators.pMalloc, 1);
	pContextHandle->pCommandBuffers =
		MakeScope<Array<VkCommandBuffer>>(g_GlobalAllocators.pMalloc, MAX_FRAMES_IN_FLIGHT);
	pContextHandle->pImageAvailableSemaphores =
		MakeScope<Array<VkSemaphore>>(g_GlobalAllocators.pMalloc, MAX_FRAMES_IN_FLIGHT);
	pContextHandle->pRenderFinishedSemaphores =
		MakeScope<Array<VkSemaphore>>(g_GlobalAllocators.pMalloc, MAX_FRAMES_IN_FLIGHT);
	pContextHandle->pInFlightFences = MakeScope<Array<VkFence>>(g_GlobalAllocators.pMalloc, MAX_FRAMES_IN_FLIGHT);

	bool deviceCreated = g_VulkanGlobals.logicalDevice != VK_NULL_HANDLE;

	VK_ASSERT(glfwCreateWindowSurface(g_VulkanGlobals.instance, pWindow, nullptr, &pContextHandle->surface));

	if (!deviceCreated)
	{
		NTT_ASSERT_RESULT_SUCCESS(chooseQueueFamilies(pContextHandle->surface));
		NTT_ASSERT_RESULT_SUCCESS(createLogicalDevice());
	}

	NTT_ASSERT_RESULT_SUCCESS(createSwapchain(pContextHandle, windowSize));

	if (!deviceCreated)
	{
		NTT_ASSERT_RESULT_SUCCESS(createRenderPass());
	}

	NTT_ASSERT_RESULT_SUCCESS(createSwapchainFramebuffers(pContextHandle));

	if (!deviceCreated)
	{
		NTT_ASSERT_RESULT_SUCCESS(createCommandPools());
	}

	NTT_ASSERT_RESULT_SUCCESS(createCommandBuffers(pContextHandle));
	NTT_ASSERT_RESULT_SUCCESS(createSyncObjects(pContextHandle));

	g_DisplayDriver.SetOnWindowResizeCallback(pWindowHandle, VulkanDriver_OnWindowResize, pContextHandle);
#else  // NTT_GLFW
	NTT_UNUSED(pWindowHandle);
	NTT_UNUSED(pRenderContextHandle);
#endif // NTT_GLFW

	return RESULT_SUCCESS;
}

static Result VulkanDriver_DestroyRenderContext(Pointer<void>& pRenderContextHandle)
{
	VulkanContextHandle* pContextHandle = VK_CONTEXT_CAST(pRenderContextHandle);

	NTT_ASSERT_RESULT_SUCCESS(destroySyncObjects(pContextHandle));
	NTT_ASSERT_RESULT_SUCCESS(destroyCommandBuffers(pContextHandle));
	NTT_ASSERT_RESULT_SUCCESS(destroySwapchainFramebuffers(pContextHandle));
	NTT_ASSERT_RESULT_SUCCESS(destroySwapchain(pContextHandle));
	vkDestroySurfaceKHR(g_VulkanGlobals.instance, pContextHandle->surface, nullptr);

	return RESULT_SUCCESS;
}

static Result recordCommandBuffer(VulkanContextHandle* pContextHandler, u32 imageIndex);
static Result endRecordCommandBuffer(VulkanContextHandle* pContextHandle);

static Result VulkanDriver_StartRender(Pointer<void> pDriverHandle)
{
	VulkanContextHandle* pContextHandle = VK_CONTEXT_CAST(pDriverHandle);

	VK_ASSERT(vkWaitForFences(g_VulkanGlobals.logicalDevice,
							  1,
							  &GET_SCOPE_ARRAY_INDEX(pContextHandle->pInFlightFences, pContextHandle->currentFrame),
							  VK_TRUE,
							  UINT64_MAX));
	VK_ASSERT(vkResetFences(g_VulkanGlobals.logicalDevice,
							1,
							&GET_SCOPE_ARRAY_INDEX(pContextHandle->pInFlightFences, pContextHandle->currentFrame)));

	u32		 imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		g_VulkanGlobals.logicalDevice,
		pContextHandle->swapchain,
		UINT64_MAX,
		GET_SCOPE_ARRAY_INDEX(pContextHandle->pImageAvailableSemaphores, pContextHandle->currentFrame),
		VK_NULL_HANDLE,
		&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		NTT_ASSERT_RESULT_SUCCESS(recreateSwapchain(pContextHandle));
		return RESULT_SUCCESS;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		NTT_VULKAN_ERROR("Failed to acquire swapchain image.");
		return RESULT_VULKAN_ERROR;
	}

	VK_ASSERT(
		vkResetCommandBuffer(GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame), 0));
	NTT_ASSERT_RESULT_SUCCESS(recordCommandBuffer(pContextHandle, imageIndex));

	pContextHandle->currentImageIndex = imageIndex;

	return RESULT_SUCCESS;
}

static Result VulkanDriver_EndRender(Pointer<void> pDriverHandle)
{
	VulkanContextHandle* pContextHandle = VK_CONTEXT_CAST(pDriverHandle);

	NTT_ASSERT_RESULT_SUCCESS(endRecordCommandBuffer(pContextHandle));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {
		GET_SCOPE_ARRAY_INDEX(pContextHandle->pImageAvailableSemaphores, pContextHandle->currentFrame)};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount	  = 1;
	submitInfo.pWaitSemaphores		  = waitSemaphores;
	submitInfo.pWaitDstStageMask	  = waitStages;
	submitInfo.commandBufferCount	  = 1;
	submitInfo.pCommandBuffers = &GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame);

	VkSemaphore signalSemaphores[] = {
		GET_SCOPE_ARRAY_INDEX(pContextHandle->pRenderFinishedSemaphores, pContextHandle->currentFrame)};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores	= signalSemaphores;

	VK_ASSERT(vkQueueSubmit(g_VulkanGlobals.graphicsQueue,
							1,
							&submitInfo,
							GET_SCOPE_ARRAY_INDEX(pContextHandle->pInFlightFences, pContextHandle->currentFrame)));

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType			   = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores	   = signalSemaphores;
	VkSwapchainKHR swapchains[]	   = {pContextHandle->swapchain};
	presentInfo.swapchainCount	   = 1;
	presentInfo.pSwapchains		   = swapchains;
	presentInfo.pImageIndices	   = &pContextHandle->currentImageIndex;

	VkResult result = vkQueuePresentKHR(g_VulkanGlobals.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		NTT_ASSERT_RESULT_SUCCESS(recreateSwapchain(pContextHandle));
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		NTT_VULKAN_ERROR("Failed to present swapchain image.");
		return RESULT_VULKAN_ERROR;
	}

	pContextHandle->currentFrame = (pContextHandle->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	vkDeviceWaitIdle(g_VulkanGlobals.logicalDevice);

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
	s_pfn##name = reinterpret_cast<PFN_vk##name>(vkGetInstanceProcAddr(g_VulkanGlobals.instance, "vk" #name));         \
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
		if (strcmp(pExtensions[i].extensionName, extensionName) == 0)
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
		if (strcmp(pLayers[i].layerName, layerName) == 0)
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
	appInfo.apiVersion		   = VK_API_VERSION_1_0;

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

	VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &g_VulkanGlobals.instance));

	NTT_VULKAN_DEBUG("Vulkan instance created.");

	return RESULT_SUCCESS;
}

static Result destroyInstance()
{
	if (g_VulkanGlobals.instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(g_VulkanGlobals.instance, nullptr);
		g_VulkanGlobals.instance = VK_NULL_HANDLE;
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

	VK_ASSERT(s_pfnCreateDebugUtilsMessengerEXT(g_VulkanGlobals.instance, &createInfo, nullptr, &g_DebugMessenger));

	NTT_VULKAN_DEBUG("Vulkan debug messenger created.");

	return RESULT_SUCCESS;
}

static Result destroyDebugMessenger()
{
	s_pfnDestroyDebugUtilsMessengerEXT(g_VulkanGlobals.instance, g_DebugMessenger, nullptr);

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
	VK_ASSERT(vkEnumeratePhysicalDevices(g_VulkanGlobals.instance, &deviceCount, nullptr));

	s_pPhysicalDevices->Resize(deviceCount);
	s_pPhysicalDeviceProperties->Resize(deviceCount);
	VK_ASSERT(vkEnumeratePhysicalDevices(g_VulkanGlobals.instance, &deviceCount, &(*s_pPhysicalDevices.Get())[0]));
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

	g_VulkanGlobals.physicalDevice = (*s_pPhysicalDevices.Get())[bestIndex];

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

static Result chooseQueueFamilies(VkSurfaceKHR surface)
{
	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(g_VulkanGlobals.physicalDevice, &queueFamilyCount, nullptr);

	if (queueFamilyCount == 0)
	{
		NTT_VULKAN_ERROR("No queue families found for the chosen physical device.");
		return RESULT_VULKAN_ERROR;
	}

	Array<VkQueueFamilyProperties> queueFamilies(queueFamilyCount, g_GlobalAllocators.pStack);
	vkGetPhysicalDeviceQueueFamilyProperties(g_VulkanGlobals.physicalDevice, &queueFamilyCount, &queueFamilies[0]);

	bool graphicsQueueFound = false;
	bool presentQueueFound	= false;

	for (u32 i = 0; i < queueFamilyCount; ++i)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFound)
		{
			g_VulkanGlobals.graphicsQueueFamilyIndex = i;
			graphicsQueueFound						 = true;
		}

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(g_VulkanGlobals.physicalDevice, i, surface, &presentSupport);
		if (presentSupport && !presentQueueFound)
		{
			g_VulkanGlobals.presentQueueFamilyIndex = i;
			presentQueueFound						= true;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			g_VulkanGlobals.transferQueueFamilyIndex = i;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			g_VulkanGlobals.computeQueueFamilyIndex = i;
		}
	}

	if (g_VulkanGlobals.graphicsQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_ERROR("No graphics queue family found for the chosen physical device.");
		return RESULT_VULKAN_ERROR;
	}
	else
	{
		NTT_VULKAN_DEBUG("Graphics queue family index: %u", g_VulkanGlobals.graphicsQueueFamilyIndex);
	}

	if (g_VulkanGlobals.presentQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_ERROR("No present queue family found for the chosen physical device.");
		return RESULT_VULKAN_ERROR;
	}
	else
	{
		NTT_VULKAN_DEBUG("Present queue family index: %u", g_VulkanGlobals.presentQueueFamilyIndex);
	}

	if (g_VulkanGlobals.transferQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_WARN("No transfer queue family found for the chosen physical device.");
	}
	else
	{
		NTT_VULKAN_DEBUG("Transfer queue family index: %u", g_VulkanGlobals.transferQueueFamilyIndex);
	}

	if (g_VulkanGlobals.computeQueueFamilyIndex == INVALID_QUEUE_FAMILY_INDEX)
	{
		NTT_VULKAN_WARN("No compute queue family found for the chosen physical device.");
	}
	else
	{
		NTT_VULKAN_DEBUG("Compute queue family index: %u", g_VulkanGlobals.computeQueueFamilyIndex);
	}

	return RESULT_SUCCESS;
}

static Result createLogicalDevice()
{
	NTT_VULKAN_DEBUG("Creating logical device for the chosen physical device.");
	float queuePriority = 1.0f;

	Array<VkDeviceQueueCreateInfo> queueCreateInfos(4, g_GlobalAllocators.pMalloc);
	u32							   queueCreateInfoCount = 0;

	VkDeviceQueueCreateInfo& currentInfo = queueCreateInfos[queueCreateInfoCount++];
	currentInfo.sType					 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	currentInfo.queueFamilyIndex		 = g_VulkanGlobals.graphicsQueueFamilyIndex;
	currentInfo.queueCount				 = 1;
	currentInfo.pQueuePriorities		 = &queuePriority;

	if (g_VulkanGlobals.presentQueueFamilyIndex != g_VulkanGlobals.graphicsQueueFamilyIndex)
	{
		VkDeviceQueueCreateInfo& presentInfo = queueCreateInfos[queueCreateInfoCount++];
		presentInfo.sType					 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		presentInfo.queueFamilyIndex		 = g_VulkanGlobals.presentQueueFamilyIndex;
		presentInfo.queueCount				 = 1;
		presentInfo.pQueuePriorities		 = &queuePriority;
	}

	if (g_VulkanGlobals.transferQueueFamilyIndex != INVALID_QUEUE_FAMILY_INDEX &&
		g_VulkanGlobals.transferQueueFamilyIndex != g_VulkanGlobals.graphicsQueueFamilyIndex &&
		g_VulkanGlobals.transferQueueFamilyIndex != g_VulkanGlobals.presentQueueFamilyIndex)
	{
		VkDeviceQueueCreateInfo& transferInfo = queueCreateInfos[queueCreateInfoCount++];
		transferInfo.sType					  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferInfo.queueFamilyIndex		  = g_VulkanGlobals.transferQueueFamilyIndex;
		transferInfo.queueCount				  = 1;
		transferInfo.pQueuePriorities		  = &queuePriority;
	}

	if (g_VulkanGlobals.computeQueueFamilyIndex != INVALID_QUEUE_FAMILY_INDEX &&
		g_VulkanGlobals.computeQueueFamilyIndex != g_VulkanGlobals.graphicsQueueFamilyIndex &&
		g_VulkanGlobals.computeQueueFamilyIndex != g_VulkanGlobals.presentQueueFamilyIndex &&
		g_VulkanGlobals.computeQueueFamilyIndex != g_VulkanGlobals.transferQueueFamilyIndex)
	{
		VkDeviceQueueCreateInfo& computeInfo = queueCreateInfos[queueCreateInfoCount++];
		computeInfo.sType					 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeInfo.queueFamilyIndex		 = g_VulkanGlobals.computeQueueFamilyIndex;
		computeInfo.queueCount				 = 1;
		computeInfo.pQueuePriorities		 = &queuePriority;
	}

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType				  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfoCount;
	deviceCreateInfo.pQueueCreateInfos	  = &queueCreateInfos[0];

	VkPhysicalDeviceFeatures enabledFeatures{};
	enabledFeatures.shaderClipDistance = VK_TRUE;
	enabledFeatures.shaderCullDistance = VK_TRUE;
	enabledFeatures.wideLines		   = VK_TRUE;
	enabledFeatures.fillModeNonSolid   = VK_TRUE;

	u32 deviceExtensionCount				 = sizeof(s_DeviceExtensions) / sizeof(s_DeviceExtensions[0]);
	deviceCreateInfo.enabledExtensionCount	 = deviceExtensionCount;
	deviceCreateInfo.ppEnabledExtensionNames = s_DeviceExtensions;
	deviceCreateInfo.pEnabledFeatures		 = &enabledFeatures;

	VK_ASSERT(
		vkCreateDevice(g_VulkanGlobals.physicalDevice, &deviceCreateInfo, nullptr, &g_VulkanGlobals.logicalDevice));

	vkGetDeviceQueue(
		g_VulkanGlobals.logicalDevice, g_VulkanGlobals.graphicsQueueFamilyIndex, 0, &g_VulkanGlobals.graphicsQueue);
	vkGetDeviceQueue(
		g_VulkanGlobals.logicalDevice, g_VulkanGlobals.presentQueueFamilyIndex, 0, &g_VulkanGlobals.presentQueue);
	vkGetDeviceQueue(
		g_VulkanGlobals.logicalDevice, g_VulkanGlobals.transferQueueFamilyIndex, 0, &g_VulkanGlobals.transferQueue);

	return RESULT_SUCCESS;
}

static Result destroyLogicalDevice()
{
	vkDestroyDevice(g_VulkanGlobals.logicalDevice, nullptr);

	NTT_VULKAN_DEBUG("Logical device is destroyed.");
	return RESULT_SUCCESS;
}

struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR  capabilities;
	Array<VkSurfaceFormatKHR> formats;
	u32						  formatCount;
	Array<VkPresentModeKHR>	  presentModes;
	u32						  presentModeCount;

	SwapchainSupportDetails()
		: capabilities{}
		, formats(1, g_GlobalAllocators.pStack)
		, formatCount(0)
		, presentModes(1, g_GlobalAllocators.pStack)
		, presentModeCount(0)
	{
	}
};

static void				  querySwapchainSupport(VulkanContextHandle* pContextHandle, SwapchainSupportDetails& out);
static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const Array<VkSurfaceFormatKHR>& availableFormats);
static VkPresentModeKHR	  chooseSwapPresentMode(const Array<VkPresentModeKHR>& availablePresentModes);
static VkExtent2D		  chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, u32 width, u32 height);
static u32				  getSwapchainImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
static Result			  createSwapchainImageViews(VulkanContextHandle* pContextHandle);

static Result createSwapchain(VulkanContextHandle* pContextHandle, Vec2u& pWindowSize)
{
	SwapchainSupportDetails swapchainSupport;
	querySwapchainSupport(pContextHandle, swapchainSupport);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR   presentMode	 = chooseSwapPresentMode(swapchainSupport.presentModes);
	VkExtent2D		   extent		 = chooseSwapExtent(swapchainSupport.capabilities, pWindowSize[0], pWindowSize[1]);
	u32				   imageCount	 = getSwapchainImageCount(swapchainSupport.capabilities);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface			= pContextHandle->surface;
	createInfo.minImageCount	= imageCount;
	createInfo.imageFormat		= surfaceFormat.format;
	createInfo.imageColorSpace	= surfaceFormat.colorSpace;
	createInfo.imageExtent		= extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform		= swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode		= presentMode;
	createInfo.clipped			= VK_TRUE;
	createInfo.oldSwapchain		= VK_NULL_HANDLE;

	u32 queueFamilyIndices[] = {g_VulkanGlobals.graphicsQueueFamilyIndex, g_VulkanGlobals.presentQueueFamilyIndex};
	if (g_VulkanGlobals.graphicsQueueFamilyIndex != g_VulkanGlobals.presentQueueFamilyIndex)
	{
		createInfo.imageSharingMode		 = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices	 = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode		 = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices	 = nullptr;
	}

	VK_ASSERT(vkCreateSwapchainKHR(g_VulkanGlobals.logicalDevice, &createInfo, nullptr, &pContextHandle->swapchain));

	vkGetSwapchainImagesKHR(
		g_VulkanGlobals.logicalDevice, pContextHandle->swapchain, &pContextHandle->swapchainImageCount, nullptr);
	pContextHandle->pSwapchainImages->Resize(pContextHandle->swapchainImageCount);
	vkGetSwapchainImagesKHR(g_VulkanGlobals.logicalDevice,
							pContextHandle->swapchain,
							&pContextHandle->swapchainImageCount,
							&GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainImages, 0));
	pContextHandle->pSwapchainImageViews->Resize(pContextHandle->swapchainImageCount);
	pContextHandle->pSwapchainFramebuffers->Resize(pContextHandle->swapchainImageCount);

	g_VulkanGlobals.swapchainImageFormat = surfaceFormat.format;
	pContextHandle->swapchainExtent		 = extent;
	g_VulkanGlobals.swapchainExtent		 = extent;
	NTT_ASSERT_RESULT_SUCCESS(createSwapchainImageViews(pContextHandle));

	NTT_VULKAN_DEBUG("Created swapchain.");
	return RESULT_SUCCESS;
}

static void querySwapchainSupport(VulkanContextHandle* pContextHandle, SwapchainSupportDetails& out)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		g_VulkanGlobals.physicalDevice, pContextHandle->surface, &out.capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(
		g_VulkanGlobals.physicalDevice, pContextHandle->surface, &out.formatCount, nullptr);
	out.formats.Resize(out.formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		g_VulkanGlobals.physicalDevice, pContextHandle->surface, &out.formatCount, &out.formats[0]);

	vkGetPhysicalDeviceSurfacePresentModesKHR(
		g_VulkanGlobals.physicalDevice, pContextHandle->surface, &out.presentModeCount, nullptr);
	out.presentModes.Resize(out.presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		g_VulkanGlobals.physicalDevice, pContextHandle->surface, &out.presentModeCount, &out.presentModes[0]);
}

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const Array<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(const Array<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, u32 width, u32 height)
{
	if (capabilities.currentExtent.width == UINT32_MAX)
	{
		VkExtent2D actualExtent = {width, height};

		actualExtent.width =
			MAX(capabilities.minImageExtent.width, MIN(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height =
			MAX(capabilities.minImageExtent.height, MIN(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

	return capabilities.currentExtent;
}

static u32 getSwapchainImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
	u32 imageCount = capabilities.minImageCount + 1;

	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	return imageCount;
}

static Result createSwapchainImageViews(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < pContextHandle->swapchainImageCount; ++i)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image						   = (*(pContextHandle->pSwapchainImages.Get()))[i];
		createInfo.viewType						   = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format						   = g_VulkanGlobals.swapchainImageFormat;
		createInfo.components.r					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a					   = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = 1;

		VK_ASSERT(vkCreateImageView(g_VulkanGlobals.logicalDevice,
									&createInfo,
									nullptr,
									&GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainImageViews, i)));
	}

	NTT_VULKAN_DEBUG("Created swapchain image views.");

	return RESULT_SUCCESS;
}

static Result destroySwapchain(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < pContextHandle->swapchainImageCount; ++i)
	{
		vkDestroyImageView(
			g_VulkanGlobals.logicalDevice, GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainImageViews, i), nullptr);
	}

	vkDestroySwapchainKHR(g_VulkanGlobals.logicalDevice, pContextHandle->swapchain, nullptr);

	pContextHandle->pSwapchainImages.Reset();
	pContextHandle->pSwapchainImageViews.Reset();
	NTT_VULKAN_DEBUG("Swapchain is destroyed.");
	return RESULT_SUCCESS;
}

static Result createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format		   = g_VulkanGlobals.swapchainImageFormat;
	colorAttachment.samples		   = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp		   = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp		   = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout	   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout	  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint	 = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments	 = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType		   = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments	   = &colorAttachment;
	renderPassInfo.subpassCount	   = 1;
	renderPassInfo.pSubpasses	   = &subpass;

	VK_ASSERT(vkCreateRenderPass(g_VulkanGlobals.logicalDevice, &renderPassInfo, nullptr, &g_VulkanGlobals.renderPass));

	NTT_VULKAN_DEBUG("Created render pass.");

	return RESULT_SUCCESS;
}

static Result destroyRenderPass()
{
	vkDestroyRenderPass(g_VulkanGlobals.logicalDevice, g_VulkanGlobals.renderPass, nullptr);
	NTT_VULKAN_DEBUG("Render pass is destroyed.");
	return RESULT_SUCCESS;
}

static Result createSwapchainFramebuffers(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < pContextHandle->swapchainImageCount; ++i)
	{
		VkImageView attachments[] = {GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainImageViews, i)};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass		= g_VulkanGlobals.renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments	= attachments;
		framebufferInfo.width			= pContextHandle->swapchainExtent.width;
		framebufferInfo.height			= pContextHandle->swapchainExtent.height;
		framebufferInfo.layers			= 1;

		VK_ASSERT(vkCreateFramebuffer(g_VulkanGlobals.logicalDevice,
									  &framebufferInfo,
									  nullptr,
									  &GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainFramebuffers, i)));
	}

	NTT_VULKAN_DEBUG("Created swapchain framebuffers.");
	return RESULT_SUCCESS;
}
static Result destroySwapchainFramebuffers(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < pContextHandle->swapchainImageCount; ++i)
	{
		vkDestroyFramebuffer(
			g_VulkanGlobals.logicalDevice, GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainFramebuffers, i), nullptr);
	}

	pContextHandle->pSwapchainFramebuffers.Reset();
	NTT_VULKAN_DEBUG("Destroyed swapchain framebuffers.");
	return RESULT_SUCCESS;
}

static Result createCommandPools()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = g_VulkanGlobals.graphicsQueueFamilyIndex;
	poolInfo.flags			  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_ASSERT(
		vkCreateCommandPool(g_VulkanGlobals.logicalDevice, &poolInfo, nullptr, &g_VulkanGlobals.graphicsCommandPool));

	if (g_VulkanGlobals.presentQueueFamilyIndex != g_VulkanGlobals.graphicsQueueFamilyIndex)
	{
		VkCommandPoolCreateInfo presentPoolInfo{};
		presentPoolInfo.sType			 = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		presentPoolInfo.queueFamilyIndex = g_VulkanGlobals.presentQueueFamilyIndex;
		presentPoolInfo.flags			 = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_ASSERT(vkCreateCommandPool(
			g_VulkanGlobals.logicalDevice, &presentPoolInfo, nullptr, &g_VulkanGlobals.presentCommandPool));
	}
	else
	{
		g_VulkanGlobals.presentCommandPool = g_VulkanGlobals.graphicsCommandPool;
	}

	if (g_VulkanGlobals.transferQueueFamilyIndex != INVALID_QUEUE_FAMILY_INDEX)
	{
		if (g_VulkanGlobals.transferQueueFamilyIndex == g_VulkanGlobals.graphicsQueueFamilyIndex)
		{
			g_VulkanGlobals.transferCommandPool = g_VulkanGlobals.graphicsCommandPool;
		}
		else if (g_VulkanGlobals.transferQueueFamilyIndex == g_VulkanGlobals.presentQueueFamilyIndex)
		{
			g_VulkanGlobals.transferCommandPool = g_VulkanGlobals.presentCommandPool;
		}
		else
		{
			VkCommandPoolCreateInfo transferPoolInfo{};
			transferPoolInfo.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			transferPoolInfo.queueFamilyIndex = g_VulkanGlobals.transferQueueFamilyIndex;
			transferPoolInfo.flags			  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			VK_ASSERT(vkCreateCommandPool(
				g_VulkanGlobals.logicalDevice, &transferPoolInfo, nullptr, &g_VulkanGlobals.transferCommandPool));
		}
	}

	NTT_VULKAN_DEBUG("Created command pools.");

	return RESULT_SUCCESS;
}

static Result destroyCommandPools()
{
	vkDestroyCommandPool(g_VulkanGlobals.logicalDevice, g_VulkanGlobals.graphicsCommandPool, nullptr);
	if (g_VulkanGlobals.presentCommandPool != g_VulkanGlobals.graphicsCommandPool)
	{
		vkDestroyCommandPool(g_VulkanGlobals.logicalDevice, g_VulkanGlobals.presentCommandPool, nullptr);
	}

	if (g_VulkanGlobals.transferCommandPool != g_VulkanGlobals.graphicsCommandPool &&
		g_VulkanGlobals.transferCommandPool != g_VulkanGlobals.presentCommandPool)
	{
		vkDestroyCommandPool(g_VulkanGlobals.logicalDevice, g_VulkanGlobals.transferCommandPool, nullptr);
	}

	NTT_VULKAN_DEBUG("Destroyed command pools.");
	return RESULT_SUCCESS;
}

static Result createCommandBuffers(VulkanContextHandle* pContextHandle)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType				 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool		 = g_VulkanGlobals.graphicsCommandPool;
	allocInfo.level				 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
	VK_ASSERT(vkAllocateCommandBuffers(
		g_VulkanGlobals.logicalDevice, &allocInfo, &GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, 0)));

	NTT_VULKAN_DEBUG("Creating command buffers.");
	return RESULT_SUCCESS;
}

static Result destroyCommandBuffers(VulkanContextHandle* pContextHandle)
{
	vkFreeCommandBuffers(g_VulkanGlobals.logicalDevice,
						 g_VulkanGlobals.graphicsCommandPool,
						 MAX_FRAMES_IN_FLIGHT,
						 &GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, 0));
	pContextHandle->pCommandBuffers.Reset();
	NTT_VULKAN_DEBUG("Destroyed command buffers.");
	return RESULT_SUCCESS;
}

static Result createSyncObjects(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VkSemaphoreCreateInfo imageAvailableSemaphoreInfo{};
		imageAvailableSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_ASSERT(vkCreateSemaphore(g_VulkanGlobals.logicalDevice,
									&imageAvailableSemaphoreInfo,
									nullptr,
									&GET_SCOPE_ARRAY_INDEX(pContextHandle->pImageAvailableSemaphores, i)));

		VkSemaphoreCreateInfo renderFinishedSemaphoreInfo{};
		renderFinishedSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_ASSERT(vkCreateSemaphore(g_VulkanGlobals.logicalDevice,
									&renderFinishedSemaphoreInfo,
									nullptr,
									&GET_SCOPE_ARRAY_INDEX(pContextHandle->pRenderFinishedSemaphores, i)));

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_ASSERT(vkCreateFence(g_VulkanGlobals.logicalDevice,
								&fenceInfo,
								nullptr,
								&GET_SCOPE_ARRAY_INDEX(pContextHandle->pInFlightFences, i)));
	}
	NTT_VULKAN_DEBUG("Created synchronization objects.");
	return RESULT_SUCCESS;
}

static Result destroySyncObjects(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroyFence(
			g_VulkanGlobals.logicalDevice, GET_SCOPE_ARRAY_INDEX(pContextHandle->pInFlightFences, i), nullptr);
		vkDestroySemaphore(g_VulkanGlobals.logicalDevice,
						   GET_SCOPE_ARRAY_INDEX(pContextHandle->pImageAvailableSemaphores, i),
						   nullptr);
		vkDestroySemaphore(g_VulkanGlobals.logicalDevice,
						   GET_SCOPE_ARRAY_INDEX(pContextHandle->pRenderFinishedSemaphores, i),
						   nullptr);
	}

	pContextHandle->pInFlightFences.Reset();
	pContextHandle->pImageAvailableSemaphores.Reset();
	pContextHandle->pRenderFinishedSemaphores.Reset();
	NTT_VULKAN_DEBUG("Destroyed synchronization objects.");

	return RESULT_SUCCESS;
}

static Result recordCommandBuffer(VulkanContextHandle* pContextHandle, u32 imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType			   = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags			   = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VK_ASSERT(vkBeginCommandBuffer(GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame),
								   &beginInfo));

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass		 = g_VulkanGlobals.renderPass;
	renderPassInfo.framebuffer		 = (*pContextHandle->pSwapchainFramebuffers.Get())[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = pContextHandle->swapchainExtent;

	VkClearValue clearColor{};
	clearColor.color = {
		{0.1f, 0.1f, 0.1f, 1.0f}
	};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues	   = &clearColor;

	vkCmdBeginRenderPass(GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame),
						 &renderPassInfo,
						 VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width	  = static_cast<float>(pContextHandle->swapchainExtent.width);
	viewport.height	  = static_cast<float>(pContextHandle->swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(
		GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = pContextHandle->swapchainExtent;
	vkCmdSetScissor(
		GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame), 0, 1, &scissor);

	return RESULT_SUCCESS;
}

static Result endRecordCommandBuffer(VulkanContextHandle* pContextHandle)
{
	vkCmdEndRenderPass(GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame));

	VK_ASSERT(vkEndCommandBuffer(GET_SCOPE_ARRAY_INDEX(pContextHandle->pCommandBuffers, pContextHandle->currentFrame)));
	return RESULT_SUCCESS;
}

static Result cleanupSwapchain(VulkanContextHandle* pContextHandle);

static Result recreateSwapchain(VulkanContextHandle* pContextHandle)
{
	vkDeviceWaitIdle(g_VulkanGlobals.logicalDevice);

	Vec2u newWindowSize = g_DisplayDriver.GetWindowSize(pContextHandle->pWindowHandle);

	NTT_VULKAN_DEBUG("Window resized to %ux%u. Recreating swapchain.", newWindowSize[0], newWindowSize[1]);

	NTT_ASSERT_RESULT_SUCCESS(cleanupSwapchain(pContextHandle));
	NTT_ASSERT_RESULT_SUCCESS(createSwapchain(pContextHandle, newWindowSize));
	NTT_ASSERT_RESULT_SUCCESS(createSwapchainFramebuffers(pContextHandle));
	return RESULT_SUCCESS;
}

static Result VulkanDriver_OnWindowResize(u32 width, u32 height, void* pUserData)
{
	NTT_UNUSED(width);
	NTT_UNUSED(height);
	NTT_UNUSED(pUserData);
	return RESULT_SUCCESS;
}

static Result cleanupSwapchain(VulkanContextHandle* pContextHandle)
{
	for (u32 i = 0; i < pContextHandle->swapchainImageCount; ++i)
	{
		vkDestroyFramebuffer(
			g_VulkanGlobals.logicalDevice, GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainFramebuffers, i), nullptr);
	}

	for (u32 i = 0; i < pContextHandle->swapchainImageCount; ++i)
	{
		vkDestroyImageView(
			g_VulkanGlobals.logicalDevice, GET_SCOPE_ARRAY_INDEX(pContextHandle->pSwapchainImageViews, i), nullptr);
	}

	vkDestroySwapchainKHR(g_VulkanGlobals.logicalDevice, pContextHandle->swapchain, nullptr);

	return RESULT_SUCCESS;
}

} // namespace ntt

#endif // NTT_VULKAN