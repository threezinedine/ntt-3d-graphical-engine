#include "texture_resource.h"
#include "resource_type.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ntt {

TextureResource::TextureResource(StringView filePath, IAllocator* pAllocator)
	: Resource(NTT_RESOURCE_TYPE_TEXTURE)
	, m_pAllocator(pAllocator)
	, m_FilePath(filePath)
	, m_TextureID(0)
{
}

TextureResource::~TextureResource()
{
}

Result TextureResource::LoadImpl()
{
	i32 width, height, channels;
	u8* data = stbi_load(m_FilePath.CStr(), &width, &height, &channels, 0);

	if (data)
	{
		NTT_RESOURCE_DEBUG("Successfully loaded texture from file: %s (Width: %d, Height: %d, Channels: %d)",
						   m_FilePath.CStr(),
						   width,
						   height,
						   channels);
	}
	else
	{
		NTT_RESOURCE_ERROR("Failed to load texture from file: %s. Error: %s", m_FilePath.CStr(), stbi_failure_reason());
		return RESULT_RESOURCE_LOAD_FAILED;
	}

	stbi_image_free(data);

	return RESULT_SUCCESS;
}

Result TextureResource::UnloadImpl()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
