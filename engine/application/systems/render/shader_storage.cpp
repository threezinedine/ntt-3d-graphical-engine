#include "shader_storage.h"
#include "render_globals.h"
#include "systems/render/render_system.h"
#include "systems/system_globals.h"
#include "texture_storage.h"

extern unsigned char mesh_vs_data[];
extern unsigned char mesh_fs_data[];

extern unsigned char vulkan_mesh_vs_data[];
extern unsigned char vulkan_mesh_fs_data[];

extern unsigned char line_vs_data[];
extern unsigned char line_fs_data[];

extern unsigned char vulkan_line_vs_data[];
extern unsigned char vulkan_line_fs_data[];

namespace ntt {

ShaderID defaultMeshShaderID;
#if NTT_DEBUG
ShaderID defaultDebugLineShaderID;
#endif // NTT_DEBUG

ShaderStorage::ShaderStorage(IAllocator* pAllocator)
	: m_pAllocator(pAllocator)
{
}

ShaderStorage::~ShaderStorage()
{
}

Result ShaderStorage::Initialize()
{
	m_pStorage = MakeScope<Storage<ShaderNode>>(m_pAllocator);

	NTT_ASSERT_RESULT_SUCCESS(InitializeImpl());

	return RESULT_SUCCESS;
}

Result ShaderStorage::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(ShutdownImpl());

	m_pStorage.Reset();
	return RESULT_SUCCESS;
}

Result ShaderStorage::SetupDefaultShaders()
{
#if NTT_VULKAN
	if (NTT_ARG_BOOL(USE_VULKAN))
	{
		defaultMeshShaderID = NTT_SHADER_STORAGE->AddShader(NTT_SHADER_INPUT_TOPOLOGY_TRIANGLES,
															reinterpret_cast<const char*>(vulkan_mesh_vs_data),
															reinterpret_cast<const char*>(vulkan_mesh_fs_data));

		defaultDebugLineShaderID = NTT_SHADER_STORAGE->AddShader(NTT_SHADER_INPUT_TOPOLOGY_LINES,
																 reinterpret_cast<const char*>(vulkan_line_vs_data),
																 reinterpret_cast<const char*>(vulkan_line_fs_data));
	}
	else
#endif // NTT_VULKAN
	{
		defaultMeshShaderID = NTT_SHADER_STORAGE->AddShader(NTT_SHADER_INPUT_TOPOLOGY_TRIANGLES,
															reinterpret_cast<const char*>(mesh_vs_data),
															reinterpret_cast<const char*>(mesh_fs_data));

#if NTT_DEBUG
		defaultDebugLineShaderID = NTT_SHADER_STORAGE->AddShader(NTT_SHADER_INPUT_TOPOLOGY_LINES,
																 reinterpret_cast<const char*>(line_vs_data),
																 reinterpret_cast<const char*>(line_fs_data));
#endif // NTT_DEBUG
	}

	return RESULT_SUCCESS;
}

Result ShaderStorage::RemoveDefaultShaders()
{
	if (defaultMeshShaderID == INVALID_SHADER_ID)
	{
		return RESULT_UNKNOWN;
	}

	NTT_ASSERT_RESULT_SUCCESS(NTT_SHADER_STORAGE->RemoveShader(defaultMeshShaderID));

#if NTT_DEBUG
	if (defaultDebugLineShaderID == INVALID_SHADER_ID)
	{
		return RESULT_UNKNOWN;
	}
	NTT_ASSERT_RESULT_SUCCESS(NTT_SHADER_STORAGE->RemoveShader(defaultDebugLineShaderID));
#endif // NTT_DEBUG

	for (u32 i = 0; i < m_pStorage->GetCount(); ++i)
	{
		if (m_pStorage->IsActive(i))
		{
			NTT_ASSERT_RESULT_SUCCESS(RemoveShader(i));
		}
	}

	return RESULT_SUCCESS;
}

ShaderID ShaderStorage::AddShader(ShaderInputTopology inputTopology,
								  const char*		  pVertexShaderSource,
								  const char*		  pFragmentShaderSource) noexcept
{
	ShaderID shaderID = m_pStorage->Add();

	if (shaderID == INVALID_SHADER_ID)
	{
		return INVALID_SHADER_ID;
	}

	ShaderNode* pShaderNode = m_pStorage->Get(shaderID);

	if (pShaderNode == nullptr)
	{
		return INVALID_SHADER_ID; // Return INVALID_SHADER_ID if the shader node is not found
	}

	if (GetUniformInfoSize() != 0)
	{
		for (u32 i = 0; i < 16; ++i)
		{
			Uniform& uniform	  = pShaderNode->uniforms[i];
			uniform.pInternalData = g_GlobalAllocators.pMalloc->Allocate(GetUniformInfoSize());
		}
	}

	pShaderNode->pShaderHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetShaderHandleSize());
	MemSet(pShaderNode->pShaderHandle.Get(), 0, GetShaderHandleSize());
	pShaderNode->inputTopology = inputTopology;

	Result result = AddShaderImpl(inputTopology,
								  pVertexShaderSource,
								  pFragmentShaderSource,
								  pShaderNode->pShaderHandle,
								  pShaderNode->uniforms,
								  pShaderNode->uniformCount);

	char uniformBufferName[1024] = {};

	for (u32 i = 0; i < pShaderNode->uniformCount; ++i)
	{
		const Uniform& uniform = pShaderNode->uniforms[i];
		format(uniformBufferName,
			   sizeof(uniformBufferName),
			   "\tUniform:\n\tName: %s\n\tType: %s\n\tSize: %u\n",
			   uniform.name.ToStringView().Data(),
			   ToString(uniform.type),
			   GetUniformTypeSize(uniform.type));
		NTT_RENDER_DEBUG("Add shader: \n%s", uniformBufferName);
	}

	if (pShaderNode->uniformCount == 0)
	{
		NTT_RENDER_DEBUG("Add shader: No uniforms found.");
	}

	if (result != RESULT_SUCCESS)
	{
		m_pStorage->Remove(shaderID);	   // Remove the shader node if adding the shader failed
		pShaderNode->pShaderHandle.Free(); // Free the allocated shader handle
		return result;					   // Return the specific error code to indicate failure
	}

	return shaderID;
}

Result ShaderStorage::RemoveShader(ShaderID shaderID)
{
	ShaderNode* pShaderNode = m_pStorage->Get(shaderID);

	if (pShaderNode == nullptr)
	{
		return RESULT_INACTIVE_STORAGE_INDEX; // Return RESULT_INACTIVE_STORAGE_INDEX if the shader node is not found
	}

	NTT_ASSERT_RESULT_SUCCESS(RemoveShaderImpl(pShaderNode->pShaderHandle));
	NTT_ASSERT_RESULT_SUCCESS(pShaderNode->pShaderHandle.Free()); // Free the allocated shader handle

	for (u32 i = 0; i < 16; ++i)
	{
		Uniform& uniform = pShaderNode->uniforms[i];
		if (uniform.pInternalData != nullptr)
		{
			NTT_ASSERT_RESULT_SUCCESS(uniform.pInternalData.Free());
			uniform.pInternalData = nullptr;
		}
	}

	return m_pStorage->Remove(shaderID);
}

Result ShaderStorage::UseShader(ShaderID shaderID, RenderContextID renderContextID)
{
	RenderSystem::RenderContext* pRenderContext =
		SystemGlobals::pRenderSystem->m_pRenderContextStorage->Get(renderContextID);
	ShaderNode* pShaderNode = m_pStorage->Get(shaderID);

	if (pShaderNode == nullptr)
	{
		return RESULT_INACTIVE_STORAGE_INDEX; // Return RESULT_INACTIVE_STORAGE_INDEX if the shader node is not found
	}

	NTT_ASSERT_RESULT_SUCCESS(UseShaderImpl(pShaderNode->pShaderHandle, pRenderContext->pRenderContextHandle));

	for (u32 i = 0; i < pShaderNode->uniformCount; ++i)
	{
		const Uniform& uniform = pShaderNode->uniforms[i];
		switch (uniform.type)
		{
#undef UNIFORM_TYPE_SAMPLER_DEF
#define UNIFORM_TYPE_SAMPLER_DEF(type, typeName, uppercase, glType)                                                    \
	case UNIFORM_TYPE_##uppercase: {                                                                                   \
		TextureStorage::TextureNode* pTextureNode =                                                                    \
			NTT_TEXTURE_STORAGE->m_pTextureStorage->Get(uniform.value.typeName);                                       \
		if (pTextureNode == nullptr)                                                                                   \
		{                                                                                                              \
			continue;                                                                                                  \
		}                                                                                                              \
		NTT_ASSERT_RESULT_SUCCESS(SetUniform##typeName##Impl(                                                          \
			uniform, pTextureNode->pTextureHandle, pShaderNode->pShaderHandle, pRenderContext->pRenderContextHandle)); \
		break;                                                                                                         \
	}
#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
	case UNIFORM_TYPE_##uppercase:                                                                                     \
		NTT_ASSERT_RESULT_SUCCESS(                                                                                     \
			SetUniform##typeName##Impl(uniform, pShaderNode->pShaderHandle, pRenderContext->pRenderContextHandle));    \
		break;
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF
		default:
			NTT_RENDER_ERROR("Unsupported uniform type: %d", uniform.type);
			return RESULT_UNSUPPORTED_UNIFORM_TYPE;
		}
	}

	return RESULT_SUCCESS;
}

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
	Result ShaderStorage::SetUniform##typeName(ShaderID shaderID, const char* pUniformName, type value)                \
	{                                                                                                                  \
		ShaderNode* pShaderNode = m_pStorage->Get(shaderID);                                                           \
		if (!pShaderNode)                                                                                              \
		{                                                                                                              \
			return RESULT_INACTIVE_STORAGE_INDEX;                                                                      \
		}                                                                                                              \
		bool uniformFound = false;                                                                                     \
		u32	 uniformIndex = static_cast<u32>(-1);                                                                      \
		for (u32 i = 0; i < pShaderNode->uniformCount; i++)                                                            \
		{                                                                                                              \
			if (pShaderNode->uniforms[i].name.ToStringView() == StringView(pUniformName))                              \
			{                                                                                                          \
				uniformFound = true;                                                                                   \
				uniformIndex = i;                                                                                      \
				break;                                                                                                 \
			}                                                                                                          \
		}                                                                                                              \
		if (!uniformFound)                                                                                             \
		{                                                                                                              \
			return RESULT_UNIFORM_NOT_FOUND;                                                                           \
		}                                                                                                              \
		pShaderNode->uniforms[uniformIndex].value.typeName = value;                                                    \
		return RESULT_SUCCESS;                                                                                         \
	}
#define UNIFORM_TYPE_SAMPLER_DEF(type, typeName, uppercase, glType) UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_DEF

const char* ShaderInputTopologyToString(ShaderInputTopology topology)
{
	switch (topology)
	{
#define SHADER_INPUT_TOPOLOGY_DEF(option, vkTopology, vkRasterizationMode)                                             \
	case NTT_SHADER_INPUT_TOPOLOGY_##option:                                                                           \
		return #option;
#include "shader_input_topology.def"
#undef SHADER_INPUT_TOPOLOGY_DEF
	default:
		return "Unknown";
	}
}

} // namespace ntt
