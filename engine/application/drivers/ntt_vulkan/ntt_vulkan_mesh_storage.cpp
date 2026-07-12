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

static Result createDynamicVertexBuffer(MeshHandle* pMeshHandle, Mesh& mesh);
static Result destroyDynamicVertexBuffer(MeshHandle* pMeshHandle);

static Result createStaticVertexBuffer(MeshHandle* pMeshHandle, Mesh& mesh);
static Result destroyStaticVertexBuffer(MeshHandle* pMeshHandle);

Result VulkanMeshStorage::AddMeshImpl(Mesh& mesh, Pointer<void>& pMeshHandle, bool dynamic)
{
	NTT_UNUSED(dynamic);

	MeshHandle* pHandle = VK_MESH_CAST(pMeshHandle);

	pHandle->isDynamic = dynamic;

	if (dynamic)
	{
		NTT_ASSERT_RESULT_SUCCESS(createDynamicVertexBuffer(pHandle, mesh));
	}
	else
	{
		NTT_ASSERT_RESULT_SUCCESS(createStaticVertexBuffer(pHandle, mesh));
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
										   debugSizeInBytes,
										   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	void* pDebugData;
	vkMapMemory(g_VulkanGlobals.logicalDevice, pHandle->debugVertexBufferMemory, 0, debugSizeInBytes, 0, &pDebugData);
	MemCopy(pDebugData, pDebugVertices.Get(), debugSizeInBytes);
	vkUnmapMemory(g_VulkanGlobals.logicalDevice, pHandle->debugVertexBufferMemory);
#endif

	return RESULT_SUCCESS;
}

static Result createDynamicVertexBuffer(MeshHandle* pMeshHandle, Mesh& mesh)
{
	u32 sizeInBytes = sizeof(Vertex) * mesh.vertices.GetCount();

	NTT_ASSERT_RESULT_SUCCESS(createBuffer(pMeshHandle->vertexBuffer,
										   pMeshHandle->vertexBufferMemory,
										   sizeInBytes,
										   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	void* pData;
	vkMapMemory(g_VulkanGlobals.logicalDevice, pMeshHandle->vertexBufferMemory, 0, sizeInBytes, 0, &pData);
	MemCopy(pData, &mesh.vertices[0], sizeInBytes);
	vkUnmapMemory(g_VulkanGlobals.logicalDevice, pMeshHandle->vertexBufferMemory);

	return RESULT_SUCCESS;
}

static Result createStaticVertexBuffer(MeshHandle* pMeshHandle, Mesh& mesh)
{
	u32 sizeInBytes = sizeof(Vertex) * mesh.vertices.GetCount();

	VkBuffer	   stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(stagingBuffer,
				 stagingBufferMemory,
				 sizeInBytes,
				 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* pData;
	vkMapMemory(g_VulkanGlobals.logicalDevice, stagingBufferMemory, 0, sizeInBytes, 0, &pData);
	MemCopy(pData, &mesh.vertices[0], sizeInBytes);
	vkUnmapMemory(g_VulkanGlobals.logicalDevice, stagingBufferMemory);

	createBuffer(pMeshHandle->vertexBuffer,
				 pMeshHandle->vertexBufferMemory,
				 sizeInBytes,
				 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType				 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level				 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool		 = g_VulkanGlobals.transferCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VK_ASSERT(vkAllocateCommandBuffers(g_VulkanGlobals.logicalDevice, &allocInfo, &commandBuffer));

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

	VK_ASSERT(vkQueueSubmit(g_VulkanGlobals.transferQueue, 1, &submitInfo, VK_NULL_HANDLE));
	VK_ASSERT(vkQueueWaitIdle(g_VulkanGlobals.transferQueue));

	vkFreeCommandBuffers(g_VulkanGlobals.logicalDevice, g_VulkanGlobals.transferCommandPool, 1, &commandBuffer);

	vkDestroyBuffer(g_VulkanGlobals.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(g_VulkanGlobals.logicalDevice, stagingBufferMemory, nullptr);

	return RESULT_SUCCESS;
}

Result VulkanMeshStorage::RemoveMeshImpl(const Pointer<void>& pMeshHandle)
{
	MeshHandle* pHandle = VK_MESH_CAST(pMeshHandle);

	if (pHandle->isDynamic)
	{
		NTT_ASSERT_RESULT_SUCCESS(destroyDynamicVertexBuffer(pHandle));
	}
	else
	{
		NTT_ASSERT_RESULT_SUCCESS(destroyStaticVertexBuffer(pHandle));
	}

#if NTT_DEBUG
	vkDestroyBuffer(g_VulkanGlobals.logicalDevice, pHandle->debugVertexBuffer, nullptr);
	vkFreeMemory(g_VulkanGlobals.logicalDevice, pHandle->debugVertexBufferMemory, nullptr);
#endif

	return RESULT_SUCCESS;
}

static Result destroyDynamicVertexBuffer(MeshHandle* pMeshHandle)
{
	vkDestroyBuffer(g_VulkanGlobals.logicalDevice, pMeshHandle->vertexBuffer, nullptr);
	vkFreeMemory(g_VulkanGlobals.logicalDevice, pMeshHandle->vertexBufferMemory, nullptr);

	return RESULT_SUCCESS;
}

static Result destroyStaticVertexBuffer(MeshHandle* pMeshHandle)
{
	vkDestroyBuffer(g_VulkanGlobals.logicalDevice, pMeshHandle->vertexBuffer, nullptr);
	vkFreeMemory(g_VulkanGlobals.logicalDevice, pMeshHandle->vertexBufferMemory, nullptr);

	return RESULT_SUCCESS;
}

u32 VulkanMeshStorage::GetMeshHandleSize() const
{
	return (u32)sizeof(MeshHandle);
}

} // namespace ntt

#endif // NTT_VULKAN