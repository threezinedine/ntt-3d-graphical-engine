#include "systems/render/shader_storage.h"

namespace ntt {

class OpenGLShaderStorage : public ShaderStorage
{
public:
	OpenGLShaderStorage(IAllocator* pAllocator = nullptr);
	~OpenGLShaderStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result AddShaderImpl(const Pointer<void>& pRenderContext,
								 const char*		  pVertexShaderSource,
								 const char*		  pFragmentShaderSource,
								 Pointer<void>&		  pShaderHandle,
								 Uniform*			  pUniforms,
								 u32&				  uniformCount) override;
	virtual Result UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) override;
	virtual Result RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) override;

protected:
	inline u32 GetUniformInfoSize() const override
	{
		return 0; // OpenGL does not require additional uniform info
	}

#define UNIFORM_TYPE_DEF(type, typeName, uppercase, glType)                                                            \
protected:                                                                                                             \
	virtual Result SetUniform##typeName##Impl(                                                                         \
		Uniform& uniform, type value, const Pointer<void>& pShaderHandle, const Pointer<void>& pRenderContext)         \
		override;
#include "systems/render/uniform_type.def"
#undef UNIFORM_TYPE_DEF

protected:
	virtual u32 GetShaderHandleSize() const;
};

} // namespace ntt
