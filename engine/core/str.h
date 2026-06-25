#pragma once

#include "alloc/alloc.h"
#include "common.h"
#include "containers/array.h"

#define NTT_SHORT_STRING_OPTIMIZATION_SIZE 15

namespace ntt {

class StringView;

class String
{
public:
	String(IAllocator* pAllocator = nullptr);
	String(const char* str, IAllocator* pAllocator = nullptr);
	String(const String& other) = delete;
	String(String&& other) noexcept;
	~String();

	u32				  Length() const;
	const char*		  CStr() const;
	void			  operator=(const String& other) = delete;
	void			  operator=(String&& other) noexcept;
	void			  operator=(const char* str);
	String			  operator+(const String& other) const;
	Result			  Clear();
	bool			  EndsWith(const String& suffix) const;
	StringView		  Slice(u32 start, u32 length) const;
	Array<StringView> Split(const String& delimiter) const;

private:
	bool		m_IsShortString;
	char		m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1];
	char*		m_pHeapBuffer;
	IAllocator* m_pAllocator;
};

class StringView
{
public:
	StringView(const char* str, u32 length = -1);

	StringView(const String& str)
		: m_pData(str.CStr())
		, m_Length(str.Length())
	{
	}

	StringView(String&& str)				= delete;
	StringView(const StringView& other)		= default;
	StringView(StringView&& other) noexcept = default;

	StringView& operator=(const StringView& other)	   = default;
	StringView& operator=(StringView&& other) noexcept = default;
	StringView& operator=(const String& str)		   = delete;
	StringView& operator=(String&& str)				   = delete;

	bool operator==(const StringView& other) const;
	bool operator!=(const StringView& other) const;
	bool operator==(const char* str) const;
	bool operator!=(const char* str) const;

	operator char*()
	{
		return const_cast<char*>(m_pData);
	}

	inline const char* Data() const
	{
		return m_pData;
	}

	inline u32 Length() const
	{
		return m_Length;
	}

private:
	const char* m_pData;
	u32			m_Length;
};

} // namespace ntt
