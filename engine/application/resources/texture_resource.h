#include "resource.h"

namespace ntt {

typedef u32 TextureID;

class TextureResource : public Resource
{
public:
	TextureResource(StringView filePath, IAllocator* pAllocator = nullptr);
	~TextureResource();

public:
	inline u32 GetTextureID() const
	{
		return m_TextureID;
	}

protected:
	virtual Result LoadImpl() override;
	virtual Result UnloadImpl() override;

private:
	IAllocator* m_pAllocator;
	String		m_FilePath;
	u32			m_TextureID; // Assuming texture ID is of type u32, adjust as necessary
};

} // namespace ntt