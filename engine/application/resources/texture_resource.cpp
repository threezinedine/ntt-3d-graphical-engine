#include "texture_resource.h"
#include "resource_type.h"
#include "systems/render/render_globals.h"
#include "systems/render/texture_storage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ntt {

TextureResource::TextureResource(RenderContextID contextID, StringView filePath, IAllocator* pAllocator)
	: Resource(NTT_RESOURCE_TYPE_TEXTURE)
	, m_pAllocator(pAllocator)
	, m_FilePath(filePath)
	, m_ContextID(contextID)
	, m_TextureID(INVALID_TEXTURE_ID)
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

		m_TextureID = NTT_TEXTURE_STORAGE->AddTexture(m_ContextID,
													  Vec3i{width, height, channels},
													  data,
													  {NTT_TEXTURE_WRAP_REPEAT,
													   NTT_TEXTURE_WRAP_REPEAT,
													   NTT_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR,
													   NTT_TEXTURE_FILTER_LINEAR});

		if (m_TextureID == INVALID_TEXTURE_ID)
		{
			NTT_RESOURCE_ERROR("Failed to add texture to storage for file: %s", m_FilePath.CStr());
			stbi_image_free(data);
			return RESULT_RESOURCE_LOAD_FAILED;
		}
	}
	else
	{
		stbi_image_free(data);
		NTT_RESOURCE_ERROR("Failed to load texture from file: %s. Error: %s", m_FilePath.CStr(), stbi_failure_reason());
		return RESULT_RESOURCE_LOAD_FAILED;
	}

	stbi_image_free(data);

	return RESULT_SUCCESS;
}

Result TextureResource::UnloadImpl()
{
	if (m_TextureID == INVALID_TEXTURE_ID)
	{
		NTT_RESOURCE_WARN("Attempted to unload texture resource that was not loaded: %s", m_FilePath.CStr());
		return RESULT_RESOURCE_IS_ALREADY_UNLOADED;
	}
	NTT_ASSERT_RESULT_SUCCESS(NTT_TEXTURE_STORAGE->RemoveTexture(m_TextureID));

	return RESULT_SUCCESS;
}

} // namespace ntt
