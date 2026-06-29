#include "shader_storage.h"

namespace ntt {

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
	return InitializeImpl();
}

Result ShaderStorage::Shutdown()
{
	for (u32 i = 0; i < m_pStorage->GetCount(); ++i)
	{
		if (m_pStorage->IsActive(i))
		{
			NTT_ASSERT_RESULT_SUCCESS(RemoveShader(i));
		}
	}

	m_pStorage.Reset();
	return ShutdownImpl();
}

ShaderID ShaderStorage::AddShader(const char* pVertexShaderSource, const char* pFragmentShaderSource) noexcept
{
	ShaderID shaderID = m_pStorage->Add((ShaderNode&&)ShaderNode{nullptr});

	if (shaderID == INVALID_SHADER_ID)
	{
		return RESULT_OUT_OF_IDS;
	}

	ShaderNode* pShaderNode = m_pStorage->Get(shaderID);

	if (pShaderNode == nullptr)
	{
		return RESULT_INACTIVE_STORAGE_INDEX; // Return RESULT_INACTIVE_STORAGE_INDEX if the shader node is not found
	}

	pShaderNode->pShaderHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetShaderHandleSize());

	Result result = AddShaderImpl(pVertexShaderSource, pFragmentShaderSource, pShaderNode->pShaderHandle);

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
	return m_pStorage->Remove(shaderID);
}

Result ShaderStorage::UseShader(ShaderID shaderID)
{
	ShaderNode* pShaderNode = m_pStorage->Get(shaderID);

	if (pShaderNode == nullptr)
	{
		return RESULT_INACTIVE_STORAGE_INDEX; // Return RESULT_INACTIVE_STORAGE_INDEX if the shader node is not found
	}

	return UseShaderImpl(pShaderNode->pShaderHandle);
}

} // namespace ntt
