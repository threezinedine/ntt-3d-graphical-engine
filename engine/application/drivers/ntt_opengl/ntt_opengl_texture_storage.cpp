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

Result OpenGLTextureStorage::AddTextureImpl(u8* pData, u32 size, Pointer<void>& pTextureHandle)
{
	TextureHandle* pHandle = CAST_TEXTURE_HANDLE(pTextureHandle);
	NTT_UNUSED(pHandle);
	NTT_UNUSED(pData);
	NTT_UNUSED(size);

	return RESULT_SUCCESS;
}

Result OpenGLTextureStorage::RemoveTextureImpl(const Pointer<void>& pTextureHandle)
{
	TextureHandle* pHandle = CAST_TEXTURE_HANDLE(pTextureHandle);
	NTT_UNUSED(pHandle);

	return RESULT_SUCCESS;
}

u32 OpenGLTextureStorage::GetTextureHandleSize() const
{
	return (u32)sizeof(TextureHandle);
}

} // namespace ntt
