#pragma once

#include "alloc/alloc.h"
#include "common.h"

#define NTT_SHORT_STRING_OPTIMIZATION_SIZE 15

namespace ntt {

class String
{
public:
	String(IAllocator* pAllocator = nullptr);
	String(const char* str, IAllocator* pAllocator = nullptr);
	String(const String& other) = delete;
	String(String&& other) noexcept;
	~String();

	u32			Length() const;
	const char* CStr() const;
	void		operator=(const String& other) = delete;
	void		operator=(String&& other) noexcept;
	Result		Clear();

private:
	bool		m_IsShortString;
	char		m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1];
	char*		m_pHeapBuffer;
	IAllocator* m_pAllocator;
};

} // namespace ntt
