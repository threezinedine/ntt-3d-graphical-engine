#pragma once

#include "services.h"

namespace ntt {

typedef u32		   ShaderID;
constexpr ShaderID INVALID_SHADER_ID = static_cast<u32>(-1);

extern ShaderID g_DefaultMeshShaderID;

class ShaderStorage
{
public:
	ShaderStorage(IAllocator* pAllocator = nullptr);
	~ShaderStorage();

	Result Initialize();
	Result Shutdown();

	ShaderID AddShader(const char* pVertexShaderSource, const char* pFragmentShaderSource) noexcept;
	Result	 UseShader(ShaderID shaderID);
	Result	 RemoveShader(ShaderID shaderID);

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result
	AddShaderImpl(const char* pVertexShaderSource, const char* pFragmentShaderSource, Pointer<void>& pShaderHandle) = 0;
	virtual Result UseShaderImpl(const Pointer<void>& pShaderHandle)												= 0;
	virtual Result RemoveShaderImpl(const Pointer<void>& pShaderHandle)												= 0;

protected:
	virtual u32 GetShaderHandleSize() const = 0;

private:
	struct ShaderNode
	{
		Pointer<void> pShaderHandle;
	};

private:
	IAllocator*				   m_pAllocator;
	Scope<Storage<ShaderNode>> m_pStorage;
};

} // namespace ntt