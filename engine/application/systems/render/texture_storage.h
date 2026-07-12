#pragma once
#include "render_system_types.h"
#include "services.h"

namespace ntt {

extern TextureID g_LogoTextureID;

enum TextureWrapMode
{
	NTT_TEXTURE_WRAP_REPEAT,
	NTT_TEXTURE_WRAP_CLAMP_TO_EDGE,
	NTT_TEXTURE_WRAP_MIRRORED_REPEAT,
	NTT_TEXTURE_WRAP_CLAMP_TO_BORDER
};

enum TextureFilterMode
{
	NTT_TEXTURE_FILTER_NEAREST,
	NTT_TEXTURE_FILTER_LINEAR,
	NTT_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,
	NTT_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,
	NTT_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,
	NTT_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR
};

struct TextureSample
{
	TextureWrapMode	  wrapS;
	TextureWrapMode	  wrapT;
	TextureFilterMode minFilter;
	TextureFilterMode magFilter;
};

class ShaderStorage;

class TextureStorage
{
public:
	TextureStorage(IAllocator* pAllocator = nullptr);
	virtual ~TextureStorage();

	Result Initialize();
	Result Shutdown();

	TextureID AddTexture(Vec3i textureShape, u8* pData, const TextureSample& sample) noexcept;
	Result	  SetTextureSample(TextureID textureID, const TextureSample& sample);
	Result	  RemoveTexture(TextureID textureID);

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;

	virtual Result
	AddTextureImpl(Vec3i textureShape, u8* pData, Pointer<void>& pTextureHandle, const TextureSample& sample) = 0;
	virtual Result SetTextureSampleImpl(TextureID textureID, const TextureSample& sample)					  = 0;
	virtual Result RemoveTextureImpl(const Pointer<void>& pTextureHandle)									  = 0;

protected:
	virtual u32 GetTextureHandleSize() const = 0;

public:
	struct TextureNode
	{
		TextureSample sample;
		Pointer<void> pTextureHandle;
		Vec3i		  textureShape;
	};

	friend class ShaderStorage;

private:
	Scope<Storage<TextureNode>> m_pTextureStorage;
	IAllocator*					m_pAllocator;
};

} // namespace ntt