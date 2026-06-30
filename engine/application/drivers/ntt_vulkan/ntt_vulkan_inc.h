#if NTT_VULKAN

#pragma once

#include "services.h"
#include "vulkan/vulkan.h"

#if NTT_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // NTT_GLFW

#define _VK_ASSERT(exp, ret)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		VkResult result = (exp);                                                                                       \
		if (result != VK_SUCCESS)                                                                                      \
		{                                                                                                              \
			NTT_VULKAN_ERROR("Vulkan error: %d", result);                                                              \
			return ret;                                                                                                \
		}                                                                                                              \
	} while (0)

#define VK_ASSERT_B(exp) _VK_ASSERT(exp, false)

#define VK_ASSERT(exp) _VK_ASSERT(exp, RESULT_VULKAN_ERROR)

namespace ntt {

struct MeshHandle
{
	u32 vertexBuffer;
	u32 indexBuffer;
	u32 vertexCount;
	u32 indexCount;
};

struct ShaderHandle
{
	VkShaderModule	 vertexModule;
	VkShaderModule	 fragmentModule;
	VkPipelineLayout pipelineLayout;
	VkPipeline		 pipeline;
	VkRenderPass	 renderPass;
};

#define VK_SHADER_CAST(handle)                                                                                         \
	reinterpret_cast<ShaderHandle*>(handle.Get());                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

struct VulkanContextHandle
{
	GLFWwindow*				  pWindow;
	VkSurfaceKHR			  surface;
	u32						  graphicsQueueFamilyIndex;
	u32						  presentQueueFamilyIndex;
	u32						  transferQueueFamilyIndex;
	u32						  computeQueueFamilyIndex;
	VkDevice				  logicalDevice;
	VkSwapchainKHR			  swapchain;
	Scope<Array<VkImage>>	  pSwapchainImages;
	u32						  swapchainImageCount;
	Scope<Array<VkImageView>> pSwapchainImageViews;
	VkFormat				  swapchainImageFormat;
	VkExtent2D				  swapchainExtent;
};

#define VK_CONTEXT_CAST(handle)                                                                                        \
	reinterpret_cast<VulkanContextHandle*>(handle.Get());                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

} // namespace ntt

#endif // NTT_VULKAN