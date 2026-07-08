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

#if NTT_DEBUG
	// Create debug vertex buffer for line rendering (each edge as 2 vertices)
	u32				debugVertexCount = mesh.vertices.GetCount() * 2;
	u32				debugSizeInBytes = debugVertexCount * sizeof(Vertex);
	Pointer<Vertex> pDebugVertices	 = g_GlobalAllocators.pStack->Allocate(debugSizeInBytes).Cast<Vertex>();
	for (u32 i = 0; i < mesh.vertices.GetCount(); ++i)
	{
		pDebugVertices.Get()[i * 2 + 0] = mesh.vertices[i];
		pDebugVertices.Get()[i * 2 + 1] = mesh.vertices[(i + 1) % mesh.vertices.GetCount()];
	}

	NTT_ASSERT_RESULT_SUCCESS(createBuffer(pHandle->debugVertexBuffer,
										   pHandle->debugVertexBufferMemory,
										   pVulkanContext,
										   debugSizeInBytes,
										   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	void* pDebugData;
	vkMapMemory(pVulkanContext->logicalDevice, pHandle->debugVertexBufferMemory, 0, debugSizeInBytes, 0, &pDebugData);
	MemCopy(pDebugData, pDebugVertices.Get(), debugSizeInBytes);
	vkUnmapMemory(pVulkanContext->logicalDevice, pHandle->debugVertexBufferMemory);
#endif

	return RESULT_SUCCESS;
}

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

#if NTT_DEBUG
Result VulkanMeshStorage::DrawDebugLineImpl(const Pointer<void>& pMeshHandle,
											const Pointer<void>& pRenderContext,
											u32					 lineWidth)
{
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);
	MeshHandle*			 pHandle		= VK_MESH_CAST(pMeshHandle);

	VkBuffer	 vertexBuffers[] = {pHandle->debugVertexBuffer};
	VkDeviceSize offsets[]		 = {0};
	vkCmdBindVertexBuffers(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
						   0,
						   1,
						   vertexBuffers,
						   offsets);

	vkCmdSetLineWidth(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
					  (float)lineWidth);

	vkCmdDraw(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
			  pHandle->vertexCount * 2,
			  1,
			  0,
			  0);
	return RESULT_SUCCESS;
}
#endif // NTT_DEBUG

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

#if NTT_DEBUG
	vkDestroyBuffer(pVulkanContext->logicalDevice, pHandle->debugVertexBuffer, nullptr);
	vkFreeMemory(pVulkanContext->logicalDevice, pHandle->debugVertexBufferMemory, nullptr);
#endif

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