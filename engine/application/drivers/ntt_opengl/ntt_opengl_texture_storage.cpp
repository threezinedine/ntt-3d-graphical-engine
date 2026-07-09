#include "ntt_opengl_texture_storage.h"
#include "ntt_opengl_inc.h"

namespace ntt {

OpenGLTextureStorage::OpenGLTextureStorage(IAllocator* pAllocator)
	: TextureStorage(pAllocator)
{
}

OpenGLTextureStorage::~OpenGLTextureStorage()
{
}

Result OpenGLTextureStorage::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result OpenGLTextureStorage::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

static GLenum convertWrapMode(TextureWrapMode wrapMode)
{
	switch (wrapMode)
	{
	case NTT_TEXTURE_WRAP_REPEAT:
		return GL_REPEAT;
	case NTT_TEXTURE_WRAP_CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case NTT_TEXTURE_WRAP_MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
	case NTT_TEXTURE_WRAP_CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;
	default:
		return GL_REPEAT; // Default to repeat if unknown
	}
}

static GLenum convertFilterMode(TextureFilterMode filterMode)
{
	switch (filterMode)
	{
	case NTT_TEXTURE_FILTER_NEAREST:
		return GL_NEAREST;
	case NTT_TEXTURE_FILTER_LINEAR:
		return GL_LINEAR;
	case NTT_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
		return GL_NEAREST_MIPMAP_NEAREST;
	case NTT_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
		return GL_LINEAR_MIPMAP_NEAREST;
	case NTT_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
		return GL_NEAREST_MIPMAP_LINEAR;
	case NTT_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
		return GL_LINEAR_MIPMAP_LINEAR;
	default:
		return GL_LINEAR; // Default to linear if unknown
	}
}

Result OpenGLTextureStorage::AddTextureImpl(Vec3i				 textureShape,
											u8*					 pData,
											Pointer<void>&		 pTextureHandle,
											const TextureSample& sample)
{
	TextureHandle* pHandle = CAST_TEXTURE_HANDLE(pTextureHandle);
	GL_ASSERT(glGenTextures(1, &pHandle->textureID));

	GL_ASSERT(glBindTexture(GL_TEXTURE_2D, pHandle->textureID));

	// Set texture parameters based on the provided sample
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convertWrapMode(sample.wrapS)));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convertWrapMode(sample.wrapT)));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convertFilterMode(sample.minFilter)));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convertFilterMode(sample.magFilter)));

	GL_ASSERT(glTexImage2D(GL_TEXTURE_2D,
						   0,
						   textureShape[2] == 3 ? GL_RGBA : GL_RGBA, // Assuming 3 channels means RGB, else RGBA
						   textureShape[0],
						   textureShape[1],
						   0,
						   textureShape[2] == 3 ? GL_RGB : GL_RGBA, // Assuming 3 channels means RGB, else RGBA
						   GL_UNSIGNED_BYTE,
						   pData)); // Assuming the texture is
									// 256x256 and RGBA format
	GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));

	return RESULT_SUCCESS;
}

Result OpenGLTextureStorage::SetTextureSampleImpl(TextureID textureID, const TextureSample& sample)
{
	NTT_UNUSED(textureID);
	NTT_UNUSED(sample);

	return RESULT_SUCCESS;
}

Result OpenGLTextureStorage::RemoveTextureImpl(const Pointer<void>& pTextureHandle)
{
	TextureHandle* pHandle = CAST_TEXTURE_HANDLE(pTextureHandle);

	GL_ASSERT(glDeleteTextures(1, &pHandle->textureID));

	return RESULT_SUCCESS;
}

u32 OpenGLTextureStorage::GetTextureHandleSize() const
{
	return (u32)sizeof(TextureHandle);
}

} // namespace ntt
