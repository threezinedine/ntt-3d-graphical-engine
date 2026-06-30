#if NTT_VULKAN

#include "ntt_vulkan_shader_storage.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"
#include "ntt_vulkan_inc.h"
#include "services.h"

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

Result VulkanShaderStorage::AddShaderImpl(const Pointer<void>& pRenderContext,
										  const char*		   pVertexShaderSource,
										  const char*		   pFragmentShaderSource,
										  Pointer<void>&	   pShaderHandle)
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

	NTT_ASSERT_RESULT_SUCCESS(vertexShaderSPIRV.Free());

	VkShaderModuleCreateInfo fragmentModuleCreateInfo{};
	fragmentModuleCreateInfo.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentModuleCreateInfo.codeSize = fragmentShaderSPIRV.size;
	fragmentModuleCreateInfo.pCode	  = fragmentShaderSPIRV.Get();
	VK_ASSERT(vkCreateShaderModule(
		pVulkanContext->logicalDevice, &fragmentModuleCreateInfo, nullptr, &pHandle->fragmentModule));

	NTT_ASSERT_RESULT_SUCCESS(fragmentShaderSPIRV.Free());

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

	u32 size = glslang_program_SPIRV_get_size(program);
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

Result VulkanShaderStorage::UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	NTT_UNUSED(pRenderContext);
	NTT_UNUSED(pShaderHandle);
	return RESULT_SUCCESS;
}

Result VulkanShaderStorage::RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle)
{
	ShaderHandle*		 pHandle		= VK_SHADER_CAST(pShaderHandle);
	VulkanContextHandle* pVulkanContext = VK_CONTEXT_CAST(pRenderContext);

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