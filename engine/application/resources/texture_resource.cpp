#include "texture_resource.h"
#include "resource_type.h"

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
	if (IsLoaded())
	{
		Unload();
	}
}

Result TextureResource::LoadImpl()
{
	return RESULT_SUCCESS;
}

Result TextureResource::UnloadImpl()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
