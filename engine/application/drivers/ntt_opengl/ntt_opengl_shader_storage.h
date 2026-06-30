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
								 Pointer<void>&		  pShaderHandle) override;
	virtual Result UseShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) override;
	virtual Result RemoveShaderImpl(const Pointer<void>& pRenderContext, const Pointer<void>& pShaderHandle) override;

protected:
	virtual u32 GetShaderHandleSize() const;
};

} // namespace ntt
