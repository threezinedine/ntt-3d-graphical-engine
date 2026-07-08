#if NTT_VULKAN

#pragma once

#include "services.h"
#include "vulkan/vulkan.h"

#if NTT_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // NTT_GLFW

#if NTT_ENABLE_ASSERTION
#define _VK_ASSERT(exp, ret)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		VkResult _result = (exp);                                                                                      \
		if (_result != VK_SUCCESS)                                                                                     \
		{                                                                                                              \
			NTT_VULKAN_ERROR("Vulkan error: %d", _result);                                                             \
			debug_break();                                                                                             \
			return ret;                                                                                                \
		}                                                                                                              \
	} while (0)
#else
#define _VK_ASSERT(exp, ret)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		VkResult _result = (exp);                                                                                      \
		if (_result != VK_SUCCESS)                                                                                     \
		{                                                                                                              \
			NTT_VULKAN_ERROR("Vulkan error: %d", _result);                                                             \
			return ret;                                                                                                \
		}                                                                                                              \
	} while (0)
#endif

#define VK_ASSERT_B(exp) _VK_ASSERT(exp, false)

#define VK_ASSERT(exp) _VK_ASSERT(exp, RESULT_VULKAN_ERROR)

#define MAX_FRAMES_IN_FLIGHT 3

namespace ntt {

struct MeshHandle
{
	VkBuffer	   vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	u32			   vertexCount;
	bool		   isDynamic;
#if NTT_DEBUG
	VkBuffer	   debugVertexBuffer;
	VkDeviceMemory debugVertexBufferMemory;
#endif
};

#define VK_MESH_CAST(handle)                                                                                           \
	reinterpret_cast<MeshHandle*>(handle.Get());                                                                       \
	do                                                                                                                 \
	{                                                                                                                  \
		if (handle == nullptr)                                                                                         \
		{                                                                                                              \
			return RESULT_NULL_POINTER;                                                                                \
		}                                                                                                              \
	} while (0)

struct ShaderHandle
{
	VkShaderModule				  vertexModule;
	VkShaderModule				  fragmentModule;
	VkPipelineLayout			  pipelineLayout;
	VkPipeline					  pipeline;
	VkDescriptorSetLayout		  descriptorSetLayout;
	VkDescriptorPool			  descriptorPool;
	u32							  descriptorSetLayoutBindingCount;
	Scope<Array<VkBuffer>>		  pBuffers;
	Scope<Array<VkDeviceMemory>>  pMemories;
	Scope<Array<void*>>			  pMapped;
	Scope<Array<VkDescriptorSet>> pDescriptorSets;
};

struct VulkanUniformInfo
{
	u32 binding;
	u32 offset;
	u32 size;
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
	Pointer<void>				  pWindowHandle;
	VkPhysicalDevice			  physicalDevice;
	VkSurfaceKHR				  surface;
	u32							  graphicsQueueFamilyIndex;
	u32							  presentQueueFamilyIndex;
	u32							  transferQueueFamilyIndex;
	u32							  computeQueueFamilyIndex;
	VkDevice					  logicalDevice;
	VkQueue						  graphicsQueue;
	VkQueue						  presentQueue;
	VkQueue						  transferQueue;
	VkSwapchainKHR				  swapchain;
	Scope<Array<VkImage>>		  pSwapchainImages;
	u32							  swapchainImageCount;
	Scope<Array<VkImageView>>	  pSwapchainImageViews;
	VkFormat					  swapchainImageFormat;
	VkExtent2D					  swapchainExtent;
	VkRenderPass				  renderPass;
	Scope<Array<VkFramebuffer>>	  pSwapchainFramebuffers;
	VkCommandPool				  graphicsCommandPool;
	VkCommandPool				  presentCommandPool;
	VkCommandPool				  transferCommandPool;
	Scope<Array<VkCommandBuffer>> pCommandBuffers;
	Scope<Array<VkSemaphore>>	  pImageAvailableSemaphores;
	Scope<Array<VkSemaphore>>	  pRenderFinishedSemaphores;
	Scope<Array<VkFence>>		  pInFlightFences;

	// temp attribute
	u32 currentImageIndex;
	u32 currentFrame;
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

Result createBuffer(VkBuffer&			  outBuffer,
					VkDeviceMemory&		  outBufferMemory,
					VulkanContextHandle*  pVulkanContext,
					VkDeviceSize		  size,
					VkBufferUsageFlags	  usage,
					VkMemoryPropertyFlags properties);

} // namespace ntt

#endif // NTT_VULKAN