#include "texture_storage.h"

namespace ntt {

TextureStorage::TextureStorage(IAllocator* pAllocator)
	: m_pAllocator(pAllocator)
{
}

TextureStorage::~TextureStorage()
{
}

Result TextureStorage::Initialize()
{
	m_pTextureStorage = MakeScope<Storage<TextureNode>>(m_pAllocator, 30);

	if (m_pTextureStorage == nullptr)
	{
		return RESULT_OUT_OF_MEMORY;
	}

	return InitializeImpl();
}

Result TextureStorage::Shutdown()
{
	for (u32 i = 0; i < m_pTextureStorage->GetCount(); ++i)
	{
		if (m_pTextureStorage->IsActive(i))
		{
			NTT_ASSERT_RESULT_SUCCESS(RemoveTexture(i));
		}
	}

	m_pTextureStorage.Reset();
	return ShutdownImpl();
}

TextureID TextureStorage::AddTexture(u8* pData, u32 size) noexcept
{
	TextureID	 textureID = m_pTextureStorage->Add();
	TextureNode* pNode	   = m_pTextureStorage->Get(textureID);

	NTT_ASSERT(pNode != nullptr);

	pNode->pTextureHandle = ALLOCATOR_SAFE(m_pAllocator)->Allocate(GetTextureHandleSize());

	NTT_ASSERT(AddTextureImpl(pData, size, pNode->pTextureHandle) == RESULT_SUCCESS);

	return textureID;
}

Result TextureStorage::RemoveTexture(TextureID textureID)
{
	TextureNode* pNode = m_pTextureStorage->Get(textureID);
	if (pNode == nullptr)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	NTT_ASSERT_RESULT_SUCCESS(pNode->pTextureHandle.Free());
	NTT_ASSERT(RemoveTextureImpl(pNode->pTextureHandle) == RESULT_SUCCESS);

	NTT_ASSERT(m_pTextureStorage->Remove(textureID) == RESULT_SUCCESS);

	return RESULT_SUCCESS;
}

} // namespace ntt
