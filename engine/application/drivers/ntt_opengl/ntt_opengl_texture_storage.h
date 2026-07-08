#include "systems/render/texture_storage.h"

namespace ntt {

class OpenGLTextureStorage : public TextureStorage
{
public:
	OpenGLTextureStorage(IAllocator* pAllocator = nullptr);
	~OpenGLTextureStorage();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;

	virtual Result AddTextureImpl(u8* pData, u32 size, Pointer<void>& pTextureHandle) override;
	virtual Result RemoveTextureImpl(const Pointer<void>& pTextureHandle) override;

	virtual u32 GetTextureHandleSize() const override;
};

} // namespace ntt