#if NTT_VULKAN

#include "ntt_vulkan_mesh_storage.h"
#include "ntt_vulkan_inc.h"

namespace ntt {

VulkanMeshStorage::VulkanMeshStorage(IAllocator* pAllocator)
	: MeshStorage(pAllocator)
{
}

VulkanMeshStorage::~VulkanMeshStorage()
{
}

Result VulkanMeshStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

static Result createDynamicVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle, Mesh& mesh);
static Result destroyDynamicVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle);

static Result createStaticVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle, Mesh& mesh);
static Result destroyStaticVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle);

Result VulkanMeshStorage::AddMeshImpl(Mesh&				   mesh,
									  Pointer<void>&	   pMeshHandle,
									  const Pointer<void>& pRenderContext,
									  bool				   dynamic)
{
	NTT_UNUSED(dynamic);

	MeshHandle*			 pHandle		= VK_MESH_CAST(pMeshHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

	pHandle->isDynamic = dynamic;

	if (dynamic)
	{
		NTT_ASSERT_RESULT_SUCCESS(createDynamicVertexBuffer(pVulkanContext, pHandle, mesh));
	}
	else
	{
		NTT_ASSERT_RESULT_SUCCESS(createStaticVertexBuffer(pVulkanContext, pHandle, mesh));
	}

	pHandle->vertexCount = mesh.vertices.GetCount();

	return RESULT_SUCCESS;
}

static VkMemoryRequirements getBufferMemoryRequirements(VulkanContextHandle* pVulkanContext, VkBuffer buffer);
static u32 findMemoryType(VulkanContextHandle* pVulkanContext, u32 typeFilter, VkMemoryPropertyFlags properties);

static Result createBuffer(VkBuffer&			 buffer,
						   VkDeviceMemory&		 bufferMemory,
						   VulkanContextHandle*	 pVulkanContext,
						   VkDeviceSize			 size,
						   VkBufferUsageFlags	 usage,
						   VkMemoryPropertyFlags properties);

static Result createDynamicVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle, Mesh& mesh)
{
	u32 sizeInBytes = sizeof(Vertex) * mesh.vertices.GetCount();

	NTT_ASSERT_RESULT_SUCCESS(createBuffer(pMeshHandle->vertexBuffer,
										   pMeshHandle->vertexBufferMemory,
										   pVulkanContext,
										   sizeInBytes,
										   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	void* pData;
	vkMapMemory(pVulkanContext->logicalDevice, pMeshHandle->vertexBufferMemory, 0, sizeInBytes, 0, &pData);
	MemCopy(pData, &mesh.vertices[0], sizeInBytes);
	vkUnmapMemory(pVulkanContext->logicalDevice, pMeshHandle->vertexBufferMemory);

	return RESULT_SUCCESS;
}

static Result createStaticVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle, Mesh& mesh)
{
	u32 sizeInBytes = sizeof(Vertex) * mesh.vertices.GetCount();

	VkBuffer	   stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(stagingBuffer,
				 stagingBufferMemory,
				 pVulkanContext,
				 sizeInBytes,
				 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* pData;
	vkMapMemory(pVulkanContext->logicalDevice, stagingBufferMemory, 0, sizeInBytes, 0, &pData);
	MemCopy(pData, &mesh.vertices[0], sizeInBytes);
	vkUnmapMemory(pVulkanContext->logicalDevice, stagingBufferMemory);

	createBuffer(pMeshHandle->vertexBuffer,
				 pMeshHandle->vertexBufferMemory,
				 pVulkanContext,
				 sizeInBytes,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType				 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level				 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool		 = pVulkanContext->transferCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VK_ASSERT(vkAllocateCommandBuffers(pVulkanContext->logicalDevice, &allocInfo, &commandBuffer));

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size		 = sizeInBytes;
	vkCmdCopyBuffer(commandBuffer, stagingBuffer, pMeshHandle->vertexBuffer, 1, &copyRegion);

	VkSubmitInfo submitInfo{};
	submitInfo.sType			  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers	  = &commandBuffer;

	VK_ASSERT(vkEndCommandBuffer(commandBuffer));

	VK_ASSERT(vkQueueSubmit(pVulkanContext->transferQueue, 1, &submitInfo, VK_NULL_HANDLE));
	VK_ASSERT(vkQueueWaitIdle(pVulkanContext->transferQueue));

	vkFreeCommandBuffers(pVulkanContext->logicalDevice, pVulkanContext->transferCommandPool, 1, &commandBuffer);

	vkDestroyBuffer(pVulkanContext->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(pVulkanContext->logicalDevice, stagingBufferMemory, nullptr);

	return RESULT_SUCCESS;
}

static Result createBuffer(VkBuffer&			 buffer,
						   VkDeviceMemory&		 bufferMemory,
						   VulkanContextHandle*	 pVulkanContext,
						   VkDeviceSize			 size,
						   VkBufferUsageFlags	 usage,
						   VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType		 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size		 = size;
	bufferCreateInfo.usage		 = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_ASSERT(vkCreateBuffer(pVulkanContext->logicalDevice, &bufferCreateInfo, nullptr, &buffer));

	VkMemoryRequirements memRequirements = getBufferMemoryRequirements(pVulkanContext, buffer);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(pVulkanContext, memRequirements.memoryTypeBits, properties);

	VK_ASSERT(vkAllocateMemory(pVulkanContext->logicalDevice, &allocInfo, nullptr, &bufferMemory));
	VK_ASSERT(vkBindBufferMemory(pVulkanContext->logicalDevice, buffer, bufferMemory, 0));

	return RESULT_SUCCESS;
}

static VkMemoryRequirements getBufferMemoryRequirements(VulkanContextHandle* pVulkanContext, VkBuffer buffer)
{
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(pVulkanContext->logicalDevice, buffer, &memRequirements);
	return memRequirements;
}

static u32 findMemoryType(VulkanContextHandle* pVulkanContext, u32 typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(pVulkanContext->physicalDevice, &memProperties);

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

Result VulkanMeshStorage::DrawMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext)
{
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);
	MeshHandle*			 pHandle		= VK_MESH_CAST(pMeshHandle);

	VkBuffer	 vertexBuffers[] = {pHandle->vertexBuffer};
	VkDeviceSize offsets[]		 = {0};
	vkCmdBindVertexBuffers(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
						   0,
						   1,
						   vertexBuffers,
						   offsets);

	vkCmdDraw(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
			  pHandle->vertexCount,
			  1,
			  0,
			  0);
	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::RemoveMeshImpl(const Pointer<void>& pMeshHandle, const Pointer<void>& pRenderContext)
{
	MeshHandle*			 pHandle		= VK_MESH_CAST(pMeshHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

	if (pHandle->isDynamic)
	{
		NTT_ASSERT_RESULT_SUCCESS(destroyDynamicVertexBuffer(pVulkanContext, pHandle));
	}
	else
	{
		NTT_ASSERT_RESULT_SUCCESS(destroyStaticVertexBuffer(pVulkanContext, pHandle));
	}

	return RESULT_SUCCESS;
}

static Result destroyDynamicVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle)
{
	vkDestroyBuffer(pVulkanContext->logicalDevice, pMeshHandle->vertexBuffer, nullptr);
	vkFreeMemory(pVulkanContext->logicalDevice, pMeshHandle->vertexBufferMemory, nullptr);

	return RESULT_SUCCESS;
}

static Result destroyStaticVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle)
{
	vkDestroyBuffer(pVulkanContext->logicalDevice, pMeshHandle->vertexBuffer, nullptr);
	vkFreeMemory(pVulkanContext->logicalDevice, pMeshHandle->vertexBufferMemory, nullptr);

	return RESULT_SUCCESS;
}

u32 VulkanMeshStorage::GetMeshHandleSize() const
{
	return (u32)sizeof(MeshHandle);
}

} // namespace ntt

#endif // NTT_VULKAN