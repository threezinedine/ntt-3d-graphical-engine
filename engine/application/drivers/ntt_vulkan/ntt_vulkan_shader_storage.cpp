#if NTT_VULKAN

#include "ntt_vulkan_shader_storage.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "ntt_vulkan_inc.h"
#include "services.h"
#include "spirv_reflect.h"
#include "systems/render/components/vertex.h"

namespace ntt {

VulkanShaderStorage::VulkanShaderStorage(IAllocator* pAllocator)
	: ShaderStorage(pAllocator)
{
}

VulkanShaderStorage::~VulkanShaderStorage()
{
}

Result VulkanShaderStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::ShutdownImpl()
{
	// Implementation for Vulkan-specific shutdown

	return RESULT_SUCCESS;
}

static Pointer<u32> compileShaderToSPIRV_Vulkan(glslang_stage_t stage, const char* shaderSource);
static Result		reflectShaderInputs(const Pointer<u32>&				   spirvCode,
										VkVertexInputBindingDescription*   vertexBindingDescriptions,
										u32&							   vertexBindingDescriptionCount,
										VkVertexInputAttributeDescription* vertexAttributeDescriptions,
										u32&							   vertexAttributeDescriptionCount);
static Result		reflectShaderUniforms(const Pointer<u32>&			spirvCode,
										  VkShaderStageFlagBits			stageFlags,
										  VkDescriptorSetLayoutBinding* pSetLayoutBindings,
										  u32*							pSetLayoutBindingSizes,
										  u32&							setLayoutBindingCount,
										  Uniform*						pUniforms,
										  u32&							uniformCount);

static Result createDescriptorSetLayout(VulkanContextHandle*		  pVulkanContext,
										ShaderHandle*				  pShaderHandle,
										VkDescriptorSetLayoutBinding* pSetLayoutBindings,
										u32							  setLayoutBindingCount);
static Result destroyDescriptorSetLayout(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle);

static Result createUniformBuffers(VulkanContextHandle* pVulkanContext,
								   ShaderHandle*		pShaderHandle,
								   u32*					pSetLayoutBindingSizes,
								   u32					setLayoutBindingCount);
static Result destroyUniformBuffers(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle);

static Result createDescriptorPool(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle);
static Result destroyDescriptorPool(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle);

static Result updateDescriptorSets(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle, u32 currentFrame);

Result VulkanShaderStorage::AddShaderImpl(const Pointer<void>&		pRenderContext,
										  const ShaderInputTopology inputTopology,
										  const char*				pVertexShaderSource,
										  const char*				pFragmentShaderSource,
										  Pointer<void>&			pShaderHandle,
										  Uniform*					pUniforms,
										  u32&						uniformCount)
{
	Pointer<u32> vertexShaderSPIRV	 = compileShaderToSPIRV_Vulkan(GLSLANG_STAGE_VERTEX, pVertexShaderSource);
	Pointer<u32> fragmentShaderSPIRV = compileShaderToSPIRV_Vulkan(GLSLANG_STAGE_FRAGMENT, pFragmentShaderSource);

	ShaderHandle*		 pHandle		= VK_SHADER_CAST(pShaderHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

	VkShaderModuleCreateInfo vertexModuleCreateInfo{};
	vertexModuleCreateInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertexModuleCreateInfo.codeSize = vertexShaderSPIRV.size;
	vertexModuleCreateInfo.pCode	= vertexShaderSPIRV.Get();
	VK_ASSERT(
		vkCreateShaderModule(pVulkanContext->logicalDevice, &vertexModuleCreateInfo, nullptr, &pHandle->vertexModule));

	VkShaderModuleCreateInfo fragmentModuleCreateInfo{};
	fragmentModuleCreateInfo.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentModuleCreateInfo.codeSize = fragmentShaderSPIRV.size;
	fragmentModuleCreateInfo.pCode	  = fragmentShaderSPIRV.Get();
	VK_ASSERT(vkCreateShaderModule(
		pVulkanContext->logicalDevice, &fragmentModuleCreateInfo, nullptr, &pHandle->fragmentModule));

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType	 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage	 = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = pHandle->vertexModule;
	vertexShaderStageInfo.pName	 = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = pHandle->fragmentModule;
	fragmentShaderStageInfo.pName  = "main";

	VkDynamicState dynamicStates[3]	 = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	u32			   dynamicStateCount = 2;

	if (inputTopology == NTT_SHADER_INPUT_TOPOLOGY_LINES)
	{
		dynamicStates[dynamicStateCount++] = VK_DYNAMIC_STATE_LINE_WIDTH;
	}

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.sType			 = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
	dynamicStateCreateInfo.pDynamicStates	 = dynamicStates;

	u32								vertexBindingDescriptionCount = 0;
	VkVertexInputBindingDescription vertexBindingDescriptions[16] = {};
	MemSet(vertexBindingDescriptions, 0, sizeof(vertexBindingDescriptions));
	u32								  vertexAttributeDescriptionCount = 0;
	VkVertexInputAttributeDescription vertexAttributeDescriptions[16] = {};
	MemSet(vertexAttributeDescriptions, 0, sizeof(vertexAttributeDescriptions));

	u32 descriptorSetLayoutBindingCount = 0;
	u32 descriptorSetLayoutSizes[16]	= {}; // Array to hold the sizes of each descriptor set layout
	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[16] = {};

	NTT_ASSERT_RESULT_SUCCESS(reflectShaderInputs(vertexShaderSPIRV,
												  vertexBindingDescriptions,
												  vertexBindingDescriptionCount,
												  vertexAttributeDescriptions,
												  vertexAttributeDescriptionCount));
	NTT_ASSERT_RESULT_SUCCESS(reflectShaderUniforms(vertexShaderSPIRV,
													VK_SHADER_STAGE_VERTEX_BIT,
													descriptorSetLayoutBindings,
													descriptorSetLayoutSizes,
													descriptorSetLayoutBindingCount,
													pUniforms,
													uniformCount));
	NTT_ASSERT_RESULT_SUCCESS(reflectShaderUniforms(fragmentShaderSPIRV,
													VK_SHADER_STAGE_FRAGMENT_BIT,
													descriptorSetLayoutBindings,
													descriptorSetLayoutSizes,
													descriptorSetLayoutBindingCount,
													pUniforms,
													uniformCount));
	NTT_ASSERT_RESULT_SUCCESS(vertexShaderSPIRV.Free());
	NTT_ASSERT_RESULT_SUCCESS(fragmentShaderSPIRV.Free());

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount	= vertexBindingDescriptionCount;
	vertexInputInfo.pVertexBindingDescriptions		= vertexBindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
	vertexInputInfo.pVertexAttributeDescriptions	= vertexAttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
#define SHADER_INPUT_TOPOLOGY_DEF(option, vkTopology, vkRasterizationMode)                                             \
	if (inputTopology == NTT_SHADER_INPUT_TOPOLOGY_##option)                                                           \
	{                                                                                                                  \
		inputAssembly.topology = vkTopology;                                                                           \
	}
#include "systems/render/shader_input_topology.def"
#undef SHADER_INPUT_TOPOLOGY_DEF
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width	  = static_cast<float>(pVulkanContext->swapchainExtent.width);
	viewport.height	  = static_cast<float>(pVulkanContext->swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = pVulkanContext->swapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports	= &viewport;
	viewportState.scissorCount	= 1;
	viewportState.pScissors		= &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType				   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable		   = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;

#define SHADER_INPUT_TOPOLOGY_DEF(option, vkTopology, vkRasterizationMode)                                             \
	if (inputTopology == NTT_SHADER_INPUT_TOPOLOGY_##option)                                                           \
	{                                                                                                                  \
		rasterizer.polygonMode = vkRasterizationMode;                                                                  \
	}
#include "systems/render/shader_input_topology.def"
#undef SHADER_INPUT_TOPOLOGY_DEF
	rasterizer.lineWidth	   = 1.0f;
	rasterizer.cullMode		   = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace	   = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType				   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable  = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType			  = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable	  = VK_FALSE;
	colorBlending.logicOp		  = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments	  = &colorBlendAttachment;

	// Descriptor set layout must be created before the pipeline layout
	NTT_ASSERT_RESULT_SUCCESS(createDescriptorSetLayout(
		pVulkanContext, pHandle, descriptorSetLayoutBindings, descriptorSetLayoutBindingCount));
	pHandle->descriptorSetLayoutBindingCount = descriptorSetLayoutBindingCount;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType		  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = pHandle->descriptorSetLayoutBindingCount > 0 ? 1 : 0;
	pipelineLayoutInfo.pSetLayouts =
		pHandle->descriptorSetLayoutBindingCount > 0 ? &pHandle->descriptorSetLayout : nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges	  = nullptr;

	VK_ASSERT(
		vkCreatePipelineLayout(pVulkanContext->logicalDevice, &pipelineLayoutInfo, nullptr, &pHandle->pipelineLayout));

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType							   = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount						   = 2;
	VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};
	pipelineInfo.pStages						   = shaderStages;
	pipelineInfo.pVertexInputState				   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState			   = &inputAssembly;
	pipelineInfo.pViewportState					   = &viewportState;
	pipelineInfo.pRasterizationState			   = &rasterizer;
	pipelineInfo.pMultisampleState				   = &multisampling;
	pipelineInfo.pDepthStencilState				   = nullptr;
	pipelineInfo.pColorBlendState				   = &colorBlending;
	pipelineInfo.pDynamicState					   = &dynamicStateCreateInfo;
	pipelineInfo.layout							   = pHandle->pipelineLayout;
	pipelineInfo.renderPass						   = pVulkanContext->renderPass;
	pipelineInfo.subpass						   = 0;

	VK_ASSERT(vkCreateGraphicsPipelines(
		pVulkanContext->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pHandle->pipeline));

	NTT_ASSERT_RESULT_SUCCESS(
		createUniformBuffers(pVulkanContext, pHandle, descriptorSetLayoutSizes, descriptorSetLayoutBindingCount));

	NTT_ASSERT_RESULT_SUCCESS(createDescriptorPool(pVulkanContext, pHandle));

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		NTT_ASSERT_RESULT_SUCCESS(updateDescriptorSets(pVulkanContext, pHandle, i));
	}

	return RESULT_SUCCESS;
}

static Result createDescriptorPool(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle)
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type			 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = pShaderHandle->descriptorSetLayoutBindingCount * MAX_FRAMES_IN_FLIGHT;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes	   = &poolSize;
	poolInfo.maxSets	   = MAX_FRAMES_IN_FLIGHT;
	VK_ASSERT(
		vkCreateDescriptorPool(pVulkanContext->logicalDevice, &poolInfo, nullptr, &pShaderHandle->descriptorPool));

	pShaderHandle->pDescriptorSets =
		MakeScope<Array<VkDescriptorSet>>(g_GlobalAllocators.pMalloc, MAX_FRAMES_IN_FLIGHT);

	Array<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, g_GlobalAllocators.pStack);

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		layouts[i] = pShaderHandle->descriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType				 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool	 = pShaderHandle->descriptorPool;
	allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
	allocInfo.pSetLayouts		 = &layouts[0];

	VK_ASSERT(vkAllocateDescriptorSets(
		pVulkanContext->logicalDevice, &allocInfo, &GET_SCOPE_ARRAY_INDEX(pShaderHandle->pDescriptorSets, 0)));

	return RESULT_SUCCESS;
}

static Result createUniformBuffers(VulkanContextHandle* pVulkanContext,
								   ShaderHandle*		pShaderHandle,
								   u32*					pSetLayoutBindingSizes,
								   u32					setLayoutBindingCount)
{
	pShaderHandle->pBuffers =
		MakeScope<Array<VkBuffer>>(g_GlobalAllocators.pMalloc, setLayoutBindingCount * MAX_FRAMES_IN_FLIGHT);
	pShaderHandle->pMemories =
		MakeScope<Array<VkDeviceMemory>>(g_GlobalAllocators.pMalloc, setLayoutBindingCount * MAX_FRAMES_IN_FLIGHT);
	pShaderHandle->pMapped =
		MakeScope<Array<void*>>(g_GlobalAllocators.pMalloc, setLayoutBindingCount * MAX_FRAMES_IN_FLIGHT);

	for (u32 i = 0; i < setLayoutBindingCount; ++i)
	{
		for (u32 j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j)
		{
			u32 bufferSize	= pSetLayoutBindingSizes[i];
			u32 bufferIndex = i * MAX_FRAMES_IN_FLIGHT + j;
			createBuffer(GET_SCOPE_ARRAY_INDEX(pShaderHandle->pBuffers, bufferIndex),
						 GET_SCOPE_ARRAY_INDEX(pShaderHandle->pMemories, bufferIndex),
						 pVulkanContext,
						 bufferSize,
						 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkMapMemory(pVulkanContext->logicalDevice,
						GET_SCOPE_ARRAY_INDEX(pShaderHandle->pMemories, bufferIndex),
						0,
						bufferSize,
						0,
						&GET_SCOPE_ARRAY_INDEX(pShaderHandle->pMapped, bufferIndex));
		}
	}

	return RESULT_SUCCESS;
}

static Result createDescriptorSetLayout(VulkanContextHandle*		  pVulkanContext,
										ShaderHandle*				  pShaderHandle,
										VkDescriptorSetLayoutBinding* pSetLayoutBindings,
										u32							  setLayoutBindingCount)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = setLayoutBindingCount;
	layoutInfo.pBindings	= pSetLayoutBindings;
	VK_ASSERT(vkCreateDescriptorSetLayout(
		pVulkanContext->logicalDevice, &layoutInfo, nullptr, &pShaderHandle->descriptorSetLayout));

	return RESULT_SUCCESS;
}

#undef UNIFORM_TYPE_SAMPLER_DEF
#define UNIFORM_TYPE_DEF(_type, typeName, uppercase, glType)                                                             \
	Result VulkanShaderStorage::SetUniform##typeName##Impl(                                                              \
		const Uniform& uniform, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderContext)                 \
	{                                                                                                                    \
		VulkanContextHandle*	   pVulkanContext	 = VK_CONTEXT_CAST(pRenderContext);                                  \
		ShaderHandle*			   pHandle			 = VK_SHADER_CAST(pShaderHandle);                                    \
		Pointer<VulkanUniformInfo> pUniformInfo		 = uniform.pInternalData.Cast<VulkanUniformInfo>();                  \
		u32						   currentFrameIndex = pVulkanContext->currentFrame;                                     \
		u32	  currentBufferIndex					 = pUniformInfo->binding * MAX_FRAMES_IN_FLIGHT + currentFrameIndex; \
		void* pMappedMemory							 = GET_SCOPE_ARRAY_INDEX(pHandle->pMapped, currentBufferIndex);      \
		MemCopy((u8*)pMappedMemory + pUniformInfo->offset, &uniform.value.typeName, pUniformInfo->size);                 \
		return RESULT_SUCCESS;                                                                                           \
	}
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF

Result VulkanShaderStorage::SetUniformSamplerImpl(const Uniform&	   uniform,
												  const Pointer<void>& pTextureHandle,
												  const Pointer<void>& pRenderContext,
												  const Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(uniform);
	NTT_UNUSED(pTextureHandle);
	NTT_UNUSED(pRenderContext);
	NTT_UNUSED(pShaderHandle);
	return RESULT_SUCCESS;
}

static Pointer<u32> compileShaderToSPIRV_Vulkan(glslang_stage_t stage, const char* shaderSource)
{
	glslang_input_t input					= {};
	input.language							= GLSLANG_SOURCE_GLSL;
	input.stage								= stage;
	input.client							= GLSLANG_CLIENT_VULKAN;
	input.client_version					= GLSLANG_TARGET_VULKAN_1_0;
	input.target_language					= GLSLANG_TARGET_SPV;
	input.target_language_version			= GLSLANG_TARGET_SPV_1_0;
	input.code								= shaderSource;
	input.default_version					= 100;
	input.default_profile					= GLSLANG_NO_PROFILE;
	input.force_default_version_and_profile = false;
	input.forward_compatible				= false;
	input.messages							= GLSLANG_MSG_DEFAULT_BIT;
	input.resource							= glslang_default_resource();

	glslang_shader_t* shader = glslang_shader_create(&input);

	Pointer<u32> bin = g_GlobalAllocators.pStack->Allocate(0).Cast<u32>();

	if (!glslang_shader_preprocess(shader, &input))
	{
		NTT_VULKAN_ERROR("GLSL preprocessing failed for shader");
		NTT_VULKAN_ERROR("%s", glslang_shader_get_info_log(shader));
		NTT_VULKAN_ERROR("%s", glslang_shader_get_info_debug_log(shader));
		NTT_VULKAN_ERROR("%s", input.code);
		glslang_shader_delete(shader);
		return bin;
	}

	if (!glslang_shader_parse(shader, &input))
	{
		NTT_VULKAN_ERROR("GLSL parsing failed for shader");
		NTT_VULKAN_ERROR("%s", glslang_shader_get_info_log(shader));
		NTT_VULKAN_ERROR("%s", glslang_shader_get_info_debug_log(shader));
		NTT_VULKAN_ERROR("%s", glslang_shader_get_preprocessed_code(shader));
		glslang_shader_delete(shader);
		return bin;
	}

	glslang_program_t* program = glslang_program_create();
	glslang_program_add_shader(program, shader);

	if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
	{
		NTT_VULKAN_ERROR("GLSL linking failed for shader");
		NTT_VULKAN_ERROR("%s", glslang_program_get_info_log(program));
		NTT_VULKAN_ERROR("%s", glslang_program_get_info_debug_log(program));
		glslang_program_delete(program);
		glslang_shader_delete(shader);
		return bin;
	}

	glslang_program_SPIRV_generate(program, stage);

	u32 size = (u32)glslang_program_SPIRV_get_size(program);
	bin		 = g_GlobalAllocators.pMalloc->Allocate(size * sizeof(u32)).Cast<u32>();
	glslang_program_SPIRV_get(program, bin.Get());

	const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
	if (spirv_messages)
	{
		NTT_VULKAN_ERROR("%s", spirv_messages);
	}

	glslang_program_delete(program);
	glslang_shader_delete(shader);

	return bin;
}

static Result reflectShaderInputs(const Pointer<u32>&				 spirvCode,
								  VkVertexInputBindingDescription*	 vertexBindingDescriptions,
								  u32&								 vertexBindingDescriptionCount,
								  VkVertexInputAttributeDescription* vertexAttributeDescriptions,
								  u32&								 vertexAttributeDescriptionCount)
{
	SpvReflectShaderModule module;
	SpvReflectResult	   result = spvReflectCreateShaderModule((size_t)spirvCode.size, spirvCode.Get(), &module);

	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		NTT_VULKAN_ERROR("Failed to create SPIR-V reflection module");
		return RESULT_VULKAN_ERROR;
	}

	// Enumerate and extract shader's input variables
	uint32_t var_count = 0;
	result			   = spvReflectEnumerateInputVariables(&module, &var_count, NULL);

	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		NTT_VULKAN_ERROR("Failed to enumerate input variables");
		spvReflectDestroyShaderModule(&module);
		return RESULT_VULKAN_ERROR;
	}

	Pointer<SpvReflectInterfaceVariable*> input_vars =
		g_GlobalAllocators.pStack->Allocate(var_count * sizeof(SpvReflectInterfaceVariable*))
			.Cast<SpvReflectInterfaceVariable*>();
	result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars.Get());
	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		NTT_VULKAN_ERROR("Failed to enumerate input variables");
		spvReflectDestroyShaderModule(&module);
		return RESULT_VULKAN_ERROR;
	}

	u32 bindingDescriptonStride = 0;
	u32 attributeCount			= 0;
	// Output variables, descriptor bindings, descriptor sets, and push constants
	for (uint32_t i = 0; i < var_count; ++i)
	{
		SpvReflectInterfaceVariable* var = input_vars.Get()[i];
		// Skip built-in variables (e.g. gl_VertexIndex) — they are not vertex attributes
		if (var->built_in != -1)
		{
			NTT_VULKAN_INFO("Skipping built-in input: %s", var->name);
			continue;
		}
		NTT_VULKAN_INFO("Input Variable: %s, Location: %u, Format: %d", var->name, var->location, var->format);
		u32 componentCount = var->numeric.vector.component_count;
		u32 componentWidth = var->numeric.scalar.width / 8;

		vertexAttributeDescriptions[attributeCount].location = var->location;
		vertexAttributeDescriptions[attributeCount].binding	 = 0; // Assuming a single binding for simplicity
		vertexAttributeDescriptions[attributeCount].format	 = static_cast<VkFormat>(var->format);
		vertexAttributeDescriptions[attributeCount].offset	 = bindingDescriptonStride;

		bindingDescriptonStride += componentCount * componentWidth;
		++attributeCount;
	}
	vertexAttributeDescriptionCount		   = attributeCount;
	vertexBindingDescriptionCount		   = 1; // Assuming a single binding for simplicity
	vertexBindingDescriptions[0].binding   = 0;
	vertexBindingDescriptions[0].stride	   = sizeof(Vertex);
	vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	// can be enumerated and extracted using a similar mechanism.

	NTT_ASSERT_RESULT_SUCCESS(input_vars.Free()); // Free the allocated memory for input variables
	// Destroy the reflection data when no longer required.
	spvReflectDestroyShaderModule(&module);
	return RESULT_SUCCESS;
}

static UniformType getUniformTypeFromFlags(const SpvReflectBlockVariable& member);

static Result reflectShaderUniforms(const Pointer<u32>&			  spirvCode,
									VkShaderStageFlagBits		  stageFlags,
									VkDescriptorSetLayoutBinding* pSetLayoutBindings,
									u32*						  pSetLayoutBindingSizes,
									u32&						  setLayoutBindingCount,
									Uniform*					  pUniforms,
									u32&						  uniformCount)
{
	SpvReflectShaderModule module;
	SpvReflectResult	   result = spvReflectCreateShaderModule((size_t)spirvCode.size, spirvCode.Get(), &module);

	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		NTT_VULKAN_ERROR("Failed to create SPIR-V reflection module");
		return RESULT_VULKAN_ERROR;
	}

	// Enumerate and extract shader's uniform variables
	uint32_t var_count = 0;
	result			   = spvReflectEnumerateDescriptorBindings(&module, &var_count, NULL);
	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		NTT_VULKAN_ERROR("Failed to enumerate descriptor bindings");
		spvReflectDestroyShaderModule(&module);
		return RESULT_VULKAN_ERROR;
	}

	Pointer<SpvReflectDescriptorBinding*> descriptor_bindings =
		g_GlobalAllocators.pStack->Allocate(var_count * sizeof(SpvReflectDescriptorBinding*))
			.Cast<SpvReflectDescriptorBinding*>();
	result = spvReflectEnumerateDescriptorBindings(&module, &var_count, descriptor_bindings.Get());
	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		NTT_VULKAN_ERROR("Failed to enumerate descriptor bindings");
		NTT_ASSERT_RESULT_SUCCESS(descriptor_bindings.Free());
		spvReflectDestroyShaderModule(&module);
		return RESULT_VULKAN_ERROR;
	}

	for (u32 i = 0; i < var_count; ++i)
	{
		SpvReflectDescriptorBinding* binding = descriptor_bindings.Get()[i];

		VkDescriptorSetLayoutBinding& layoutBinding		= pSetLayoutBindings[setLayoutBindingCount++];
		u32&						  layoutBindingSize = pSetLayoutBindingSizes[setLayoutBindingCount - 1];
		layoutBinding.binding							= binding->binding;
		layoutBinding.descriptorType					= static_cast<VkDescriptorType>(binding->descriptor_type);
		layoutBinding.descriptorCount					= binding->count;
		layoutBinding.stageFlags						= stageFlags;
		layoutBinding.pImmutableSamplers				= nullptr; // Optional
		layoutBindingSize								= binding->block.size;

		u32 uniformSize = 0;

		if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			for (u32 j = 0; j < binding->block.member_count; ++j)
			{
				SpvReflectBlockVariable& member	 = binding->block.members[j];
				Uniform&				 uniform = pUniforms[uniformCount++];
				uniform.name					 = String(member.name);

				NTT_VULKAN_INFO(
					"Uniform: %s, Type: %d, Size: %u", member.name, member.type_description->type_flags, member.size);

				uniform.type  = getUniformTypeFromFlags(member);
				uniform.value = {}; // Initialize the value as needed

				Pointer<VulkanUniformInfo> pUniformInfo = uniform.pInternalData.Cast<VulkanUniformInfo>();
				pUniformInfo->offset					= uniformSize;
				pUniformInfo->size						= GetUniformTypeSize(uniform.type);
				pUniformInfo->binding					= binding->binding;

				uniformSize += pUniformInfo->size;
			}
		}

		layoutBindingSize = binding->block.size;
		// if (layoutBindingSize != uniformSize)
		// {
		// 	NTT_VULKAN_WARN("Uniform size mismatch for binding %u: expected %u, got %u",
		// 					layoutBinding.binding,
		// 					layoutBindingSize,
		// 					uniformSize);
		// }
	}

	NTT_ASSERT_RESULT_SUCCESS(descriptor_bindings.Free()); // Free the allocated memory for descriptor bindings
	spvReflectDestroyShaderModule(&module);
	return RESULT_SUCCESS;
}

static UniformType getUniformTypeFromFlags(const SpvReflectBlockVariable& member)
{
	if (!member.type_description)
	{
		return UNIFORM_TYPE_FLOAT;
	}

	SpvReflectTypeFlags		typeFlags = member.type_description->type_flags;
	SpvReflectNumericTraits numeric	  = member.type_description->traits.numeric;

	if (typeFlags & SPV_REFLECT_TYPE_FLAG_MATRIX)
	{
		if (typeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT)
		{
			if (numeric.matrix.column_count == 4 && numeric.matrix.row_count == 4)
				return UNIFORM_TYPE_MAT4;
			else if (numeric.matrix.column_count == 3 && numeric.matrix.row_count == 3)
				return UNIFORM_TYPE_MAT3;
			else if (numeric.matrix.column_count == 2 && numeric.matrix.row_count == 2)
				return UNIFORM_TYPE_MAT2;
		}
	}

	if (typeFlags & SPV_REFLECT_TYPE_FLAG_VECTOR)
	{
		if (typeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT)
		{
			if (numeric.vector.component_count == 4)
				return UNIFORM_TYPE_FLOAT4;
			else if (numeric.vector.component_count == 3)
				return UNIFORM_TYPE_FLOAT3;
			else if (numeric.vector.component_count == 2)
				return UNIFORM_TYPE_FLOAT2;
		}
	}

	if (typeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT)
	{
		return UNIFORM_TYPE_FLOAT;
	}

	return UNIFORM_TYPE_FLOAT; // Default to float if type is unrecognized
}

Result VulkanShaderStorage::UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);
	ShaderHandle*		 pHandle		= VK_SHADER_CAST(pShaderHandle);

	vkCmdBindPipeline(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
					  VK_PIPELINE_BIND_POINT_GRAPHICS,
					  pHandle->pipeline);

	if (pHandle->pDescriptorSets.Get() != nullptr && pHandle->descriptorSetLayoutBindingCount > 0)
	{
		vkCmdBindDescriptorSets(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
								VK_PIPELINE_BIND_POINT_GRAPHICS,
								pHandle->pipelineLayout,
								0,
								1,
								&GET_SCOPE_ARRAY_INDEX(pHandle->pDescriptorSets, pVulkanContext->currentFrame),
								0,
								nullptr);
	}

	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	ShaderHandle*		 pHandle		= VK_SHADER_CAST(pShaderHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

	NTT_ASSERT_RESULT_SUCCESS(destroyDescriptorPool(pVulkanContext, pHandle));
	NTT_ASSERT_RESULT_SUCCESS(destroyUniformBuffers(pVulkanContext, pHandle));
	NTT_ASSERT_RESULT_SUCCESS(destroyDescriptorSetLayout(pVulkanContext, pHandle));
	vkDestroyPipeline(pVulkanContext->logicalDevice, pHandle->pipeline, nullptr);
	vkDestroyPipelineLayout(pVulkanContext->logicalDevice, pHandle->pipelineLayout, nullptr);
	vkDestroyShaderModule(pVulkanContext->logicalDevice, pHandle->vertexModule, nullptr);
	vkDestroyShaderModule(pVulkanContext->logicalDevice, pHandle->fragmentModule, nullptr);

	return RESULT_SUCCESS;
}

static Result destroyDescriptorPool(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle)
{
	vkDestroyDescriptorPool(pVulkanContext->logicalDevice, pShaderHandle->descriptorPool, nullptr);

	pShaderHandle->pDescriptorSets.Reset();

	return RESULT_SUCCESS;
}

static Result destroyUniformBuffers(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle)
{
	for (u32 i = 0; i < pShaderHandle->descriptorSetLayoutBindingCount; ++i)
	{
		for (u32 j = 0; j < MAX_FRAMES_IN_FLIGHT; ++j)
		{
			u32 bufferIndex = i * MAX_FRAMES_IN_FLIGHT + j;

			vkUnmapMemory(pVulkanContext->logicalDevice, GET_SCOPE_ARRAY_INDEX(pShaderHandle->pMemories, bufferIndex));

			vkDestroyBuffer(
				pVulkanContext->logicalDevice, GET_SCOPE_ARRAY_INDEX(pShaderHandle->pBuffers, bufferIndex), nullptr);
			vkFreeMemory(
				pVulkanContext->logicalDevice, GET_SCOPE_ARRAY_INDEX(pShaderHandle->pMemories, bufferIndex), nullptr);
		}
	}

	pShaderHandle->pBuffers.Reset();
	pShaderHandle->pMemories.Reset();
	pShaderHandle->pMapped.Reset();

	return RESULT_SUCCESS;
}

static Result destroyDescriptorSetLayout(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle)
{
	vkDestroyDescriptorSetLayout(pVulkanContext->logicalDevice, pShaderHandle->descriptorSetLayout, nullptr);

	return RESULT_SUCCESS;
}

u32 VulkanShaderStorage::GetShaderHandleSize() const
{
	return (u32)sizeof(ShaderHandle);
}

u32 VulkanShaderStorage::GetUniformInfoSize() const
{
	return (u32)sizeof(VulkanUniformInfo);
}

static Result updateDescriptorSets(VulkanContextHandle* pVulkanContext, ShaderHandle* pShaderHandle, u32 currentFrame)
{
	for (u32 i = 0; i < pShaderHandle->descriptorSetLayoutBindingCount; ++i)
	{
		u32 bufferIndex = i * MAX_FRAMES_IN_FLIGHT + currentFrame;

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = GET_SCOPE_ARRAY_INDEX(pShaderHandle->pBuffers, bufferIndex);
		bufferInfo.offset = 0;
		bufferInfo.range  = VK_WHOLE_SIZE;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet			= GET_SCOPE_ARRAY_INDEX(pShaderHandle->pDescriptorSets, currentFrame);
		descriptorWrite.dstBinding		= i;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo		= &bufferInfo;

		vkUpdateDescriptorSets(pVulkanContext->logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}

	return RESULT_SUCCESS;
}

} // namespace ntt

#endif // NTT_VULKAN