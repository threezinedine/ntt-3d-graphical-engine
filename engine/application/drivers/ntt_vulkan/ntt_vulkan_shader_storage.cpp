#if NTT_VULKAN

#include "ntt_vulkan_shader_storage.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "ntt_vulkan_inc.h"
#include "services.h"
#include "spirv_reflect.h"

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

Result VulkanShaderStorage::AddShaderImpl(const Pointer<void>& pRenderContext,
										  const char*		   pVertexShaderSource,
										  const char*		   pFragmentShaderSource,
										  Pointer<void>&	   pShaderHandle,
										  Uniform*			   pUniforms,
										  u32&				   uniformCount)
{
	NTT_UNUSED(pUniforms);
	NTT_UNUSED(uniformCount);

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

	VkDynamicState dynamicStates[]	 = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	u32			   dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);

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

	NTT_ASSERT_RESULT_SUCCESS(reflectShaderInputs(vertexShaderSPIRV,
												  vertexBindingDescriptions,
												  vertexBindingDescriptionCount,
												  vertexAttributeDescriptions,
												  vertexAttributeDescriptionCount));
	NTT_ASSERT_RESULT_SUCCESS(vertexShaderSPIRV.Free());
	NTT_ASSERT_RESULT_SUCCESS(fragmentShaderSPIRV.Free());

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount	= vertexBindingDescriptionCount;
	vertexInputInfo.pVertexBindingDescriptions		= vertexBindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
	vertexInputInfo.pVertexAttributeDescriptions	= vertexAttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType					 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology				 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
	rasterizer.polygonMode			   = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth			   = 1.0f;
	rasterizer.cullMode				   = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace			   = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable		   = VK_FALSE;

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

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType				  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount		  = 0;
	pipelineLayoutInfo.pSetLayouts			  = nullptr;
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

	return RESULT_SUCCESS;
}

#define UNIFORM_TYPE_DEF(type, name, uppercase, glType)                                                                \
	Result VulkanShaderStorage::SetUniform##name##Impl(                                                                \
		const char* pUniformName, type value, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderContext) \
	{                                                                                                                  \
		NTT_UNUSED(pShaderHandle);                                                                                     \
		NTT_UNUSED(pRenderContext);                                                                                    \
		NTT_UNUSED(pUniformName);                                                                                      \
		NTT_UNUSED(value);                                                                                             \
		return RESULT_SUCCESS;                                                                                         \
	}
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF

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
	vertexBindingDescriptions[0].stride	   = bindingDescriptonStride; // Set the stride based on your vertex structure
	vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	// can be enumerated and extracted using a similar mechanism.

	NTT_ASSERT_RESULT_SUCCESS(input_vars.Free()); // Free the allocated memory for input variables
	// Destroy the reflection data when no longer required.
	spvReflectDestroyShaderModule(&module);
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);
	ShaderHandle*		 pHandle		= VK_SHADER_CAST(pShaderHandle);

	vkCmdBindPipeline(GET_SCOPE_ARRAY_INDEX(pVulkanContext->pCommandBuffers, pVulkanContext->currentFrame),
					  VK_PIPELINE_BIND_POINT_GRAPHICS,
					  pHandle->pipeline);

	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	ShaderHandle*		 pHandle		= VK_SHADER_CAST(pShaderHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

	vkDestroyPipeline(pVulkanContext->logicalDevice, pHandle->pipeline, nullptr);
	vkDestroyPipelineLayout(pVulkanContext->logicalDevice, pHandle->pipelineLayout, nullptr);
	vkDestroyShaderModule(pVulkanContext->logicalDevice, pHandle->vertexModule, nullptr);
	vkDestroyShaderModule(pVulkanContext->logicalDevice, pHandle->fragmentModule, nullptr);

	return RESULT_SUCCESS;
}

u32 VulkanShaderStorage::GetShaderHandleSize() const
{
	return (u32)sizeof(ShaderHandle);
}

} // namespace ntt

#endif // NTT_VULKAN