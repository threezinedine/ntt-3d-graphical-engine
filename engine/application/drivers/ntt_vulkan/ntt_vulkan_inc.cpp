#if NTT_VULKAN

#include "ntt_vulkan_inc.h"

namespace ntt {

static VkMemoryRequirements getBufferMemoryRequirements(VkBuffer buffer)
{
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(g_VulkanGlobals.logicalDevice, buffer, &memRequirements);
	return memRequirements;
}

static u32 findMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(g_VulkanGlobals.physicalDevice, &memProperties);

	for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	NTT_ASSERT_MSG(false, "Failed to find suitable memory type.");
	return 0;
}

Result createBuffer(VkBuffer&			  outBuffer,
					VkDeviceMemory&		  outBufferMemory,
					VkDeviceSize		  size,
					VkBufferUsageFlags	  usage,
					VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType		 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size		 = size;
	bufferCreateInfo.usage		 = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_ASSERT(vkCreateBuffer(g_VulkanGlobals.logicalDevice, &bufferCreateInfo, nullptr, &outBuffer));

	VkMemoryRequirements memRequirements = getBufferMemoryRequirements(outBuffer);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VK_ASSERT(vkAllocateMemory(g_VulkanGlobals.logicalDevice, &allocInfo, nullptr, &outBufferMemory));
	VK_ASSERT(vkBindBufferMemory(g_VulkanGlobals.logicalDevice, outBuffer, outBufferMemory, 0));

	return RESULT_SUCCESS;
}

} // namespace ntt

#endif // NTT_VULKAN