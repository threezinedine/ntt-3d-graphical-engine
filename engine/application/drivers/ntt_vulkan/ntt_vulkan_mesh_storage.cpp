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

static Result createVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle, Mesh& mesh);
static Result destroyVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle);

Result VulkanMeshStorage::AddMeshImpl(Mesh&				   mesh,
									  Pointer<void>&	   pMeshHandle,
									  const Pointer<void>& pRenderContext,
									  bool				   dynamic)
{
	NTT_UNUSED(dynamic);

	MeshHandle*			 pHandle		= VK_MESH_CAST(pMeshHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

	NTT_ASSERT_RESULT_SUCCESS(createVertexBuffer(pVulkanContext, pHandle, mesh));

	return RESULT_SUCCESS;
}

static VkMemoryRequirements getBufferMemoryRequirements(VulkanContextHandle* pVulkanContext, VkBuffer buffer);
static u32 findMemoryType(VulkanContextHandle* pVulkanContext, u32 typeFilter, VkMemoryPropertyFlags properties);

static Result createVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle, Mesh& mesh)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType		 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size		 = sizeof(Vertex) * mesh.vertices.GetCount();
	bufferCreateInfo.usage		 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_ASSERT(vkCreateBuffer(pVulkanContext->logicalDevice, &bufferCreateInfo, nullptr, &pMeshHandle->vertexBuffer));

	VkMemoryRequirements memRequirements = getBufferMemoryRequirements(pVulkanContext, pMeshHandle->vertexBuffer);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType			 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex =
		findMemoryType(pVulkanContext,
					   memRequirements.memoryTypeBits,
					   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VK_ASSERT(vkAllocateMemory(pVulkanContext->logicalDevice, &allocInfo, nullptr, &pMeshHandle->vertexBufferMemory));
	VK_ASSERT(vkBindBufferMemory(
		pVulkanContext->logicalDevice, pMeshHandle->vertexBuffer, pMeshHandle->vertexBufferMemory, 0));

	void* data;
	vkMapMemory(pVulkanContext->logicalDevice, pMeshHandle->vertexBufferMemory, 0, bufferCreateInfo.size, 0, &data);
	MemCopy(data, &mesh.vertices[0], sizeof(Vertex) * mesh.vertices.GetCount());
	vkUnmapMemory(pVulkanContext->logicalDevice, pMeshHandle->vertexBufferMemory);

	pMeshHandle->vertexCount = mesh.vertices.GetCount();

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

	NTT_ASSERT_RESULT_SUCCESS(destroyVertexBuffer(pVulkanContext, pHandle));

	return RESULT_SUCCESS;
}

static Result destroyVertexBuffer(VulkanContextHandle* pVulkanContext, MeshHandle* pMeshHandle)
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