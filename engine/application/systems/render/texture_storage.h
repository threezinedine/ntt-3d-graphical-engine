#pragma once
#include "services.h"

namespace ntt {

typedef u32			TextureID;
constexpr TextureID INVALID_TEXTURE_ID = static_cast<u32>(-1);

extern TextureID g_LogoTextureID;

class TextureStorage
{
public:
	TextureStorage(IAllocator* pAllocator = nullptr);
	virtual ~TextureStorage();

	Result Initialize();
	Result Shutdown();

	TextureID AddTexture(u8* pData, u32 size) noexcept;
	Result	  RemoveTexture(TextureID textureID);

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result AddTextureImpl(u8* pData, u32 size, Pointer<void>& pTextureHandle) = 0;
	virtual Result RemoveTextureImpl(const Pointer<void>& pTextureHandle)			  = 0;

protected:
	virtual u32 GetTextureHandleSize() const = 0;

public:
	struct TextureNode
	{
		Pointer<void> pTextureHandle;
	};

private:
	Scope<Storage<TextureNode>> m_pTextureStorage;
	IAllocator*					m_pAllocator;
};

} // namespace ntt