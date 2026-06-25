#include "str.h"
#include "string.h"

namespace ntt {

String::String(IAllocator* pAllocator)
	: String("", pAllocator)
{
}

String::String(const char* str, IAllocator* pAllocator)
	: m_pHeapBuffer(nullptr)
	, m_pAllocator(pAllocator)
{
	NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before initialization

	if (strlen(str) <= NTT_SHORT_STRING_OPTIMIZATION_SIZE)
	{
		m_IsShortString = true;
		m_pHeapBuffer	= nullptr;
		memcpy(m_pShortBuffer, str, NTT_SHORT_STRING_OPTIMIZATION_SIZE);
		m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE] = '\0'; // Ensure null-termination
	}
	else
	{
		m_IsShortString = false;
		m_pHeapBuffer	= (char*)ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)strlen(str) + 1);
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		memcpy(m_pHeapBuffer, str, (u32)strlen(str) + 1);
	}
}

String::String(String&& other) noexcept
	: m_IsShortString(other.m_IsShortString)
	, m_pAllocator(other.m_pAllocator)
{
	m_pHeapBuffer = nullptr;
	NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before moving

	if (m_IsShortString)
	{
		memcpy(m_pShortBuffer, other.m_pShortBuffer, NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1);
		memset(other.m_pShortBuffer, 0, NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1); // Clear the source buffer
	}
	else
	{
		m_pHeapBuffer		= other.m_pHeapBuffer;
		other.m_pHeapBuffer = nullptr; // Prevent double free
	}

	other.m_pAllocator = nullptr; // Prevent double free of allocator if moved
}

String::~String()
{
	if (m_pHeapBuffer != nullptr)
	{
		ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer, (u32)(strlen(m_pHeapBuffer) + 1));
		m_pHeapBuffer = nullptr; // Prevent dangling pointer
	}
}

const char* String::CStr() const
{
	return m_IsShortString ? m_pShortBuffer : m_pHeapBuffer;
}

u32 String::Length() const
{
	return m_IsShortString ? (u32)strlen(m_pShortBuffer) : (u32)strlen(m_pHeapBuffer);
}

void String::operator=(String&& other) noexcept
{
	if (this != &other)
	{
		NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before moving

		// Free existing resources
		if (!m_IsShortString && m_pHeapBuffer != nullptr)
		{
			ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer, (u32)(strlen(m_pHeapBuffer) + 1));
		}

		m_IsShortString = other.m_IsShortString;

		if (m_IsShortString)
		{
			memcpy(m_pShortBuffer, other.m_pShortBuffer, NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1);
			memset(other.m_pShortBuffer, 0, NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1); // Clear the source buffer
		}
		else
		{
			m_pHeapBuffer		= other.m_pHeapBuffer;
			other.m_pHeapBuffer = nullptr; // Prevent double free
		}

		m_pAllocator	   = other.m_pAllocator;
		other.m_pAllocator = nullptr; // Prevent double free of allocator if moved
	}
}

void String::operator=(const char* str)
{
	NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before assignment

	if (strlen(str) <= NTT_SHORT_STRING_OPTIMIZATION_SIZE)
	{
		m_IsShortString = true;
		m_pHeapBuffer	= nullptr;
		memcpy(m_pShortBuffer, str, NTT_SHORT_STRING_OPTIMIZATION_SIZE);
		m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE] = '\0'; // Ensure null-termination
	}
	else
	{
		m_IsShortString = false;
		m_pHeapBuffer	= (char*)ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)strlen(str) + 1);
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		memcpy(m_pHeapBuffer, str, (u32)strlen(str) + 1);
	}
}

Result String::Clear()
{
	m_IsShortString = true;
	memset(m_pShortBuffer, 0, sizeof(m_pShortBuffer));
	if (m_pHeapBuffer != nullptr)
	{
		NTT_ASSERT_RESULT_SUCCESS(ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer, (u32)strlen(m_pHeapBuffer) + 1));
		m_pHeapBuffer = nullptr;
	}
	return RESULT_SUCCESS;
}

String String::operator+(const String& other) const
{
	NTT_ASSERT(ALLOCATOR_SAFE(m_pAllocator) == ALLOCATOR_SAFE(other.m_pAllocator));

	u32 newLength = Length() + other.Length();
	if (newLength <= NTT_SHORT_STRING_OPTIMIZATION_SIZE)
	{
		String result(m_pAllocator);
		result.m_IsShortString = true;
		memcpy(result.m_pShortBuffer, CStr(), NTT_SHORT_STRING_OPTIMIZATION_SIZE);
		memcpy(result.m_pShortBuffer + strlen(result.m_pShortBuffer),
			   other.CStr(),
			   NTT_SHORT_STRING_OPTIMIZATION_SIZE - strlen(result.m_pShortBuffer));
		result.m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE] = '\0'; // Ensure null-termination
		return result;
	}
	else
	{
		String result(m_pAllocator);
		result.m_IsShortString = false;
		result.m_pHeapBuffer   = (char*)ALLOCATOR_SAFE(m_pAllocator)->Allocate(newLength + 1);
		NTT_ASSERT_MSG(result.m_pHeapBuffer != nullptr, "Failed to allocate memory for concatenated string.");
		memcpy(result.m_pHeapBuffer, CStr(), (size_t)Length());
		memcpy(result.m_pHeapBuffer + (size_t)Length(), other.CStr(), (size_t)other.Length() + 1);
		return result;
	}
}

bool String::EndsWith(const String& suffix) const
{
	u32 strLength	 = Length();
	u32 suffixLength = suffix.Length();

	if (suffixLength > strLength)
	{
		return false;
	}

	const char* strPtr	  = CStr() + (strLength - suffixLength);
	const char* suffixPtr = suffix.CStr();

	return strcmp(strPtr, suffixPtr) == 0;
}

StringView String::Slice(u32 start, u32 length) const
{
	if (start >= Length())
	{
		return StringView(nullptr, 0); // Return an empty StringView if start is out of bounds
	}

	u32 end = start + length;
	if (end > Length())
	{
		end = Length(); // Adjust end if it exceeds the string length
	}

	return StringView(CStr() + start, end - start);
}

Array<StringView> String::Split(const String& delimiter) const
{
	Array<StringView> result;
	const char*		  strPtr		  = CStr();
	const char*		  delimPtr		  = delimiter.CStr();
	u32				  delimiterLength = delimiter.Length();
	u32				  strLength		  = Length();
	u32				  baseIndex		  = 0;

	for (u32 i = 0; i < strLength; i++)
	{
		if (strncmp(strPtr + i, delimPtr, delimiterLength) == 0)
		{
			result.Append(StringView(strPtr + baseIndex, i - baseIndex));
			baseIndex = i + delimiterLength;
		}
	}

	if (baseIndex <= strLength)
	{
		result.Append(StringView(strPtr + baseIndex, strLength - baseIndex));
	}

	return result;
}

bool StringView::operator==(const StringView& other) const
{
	return m_Length == other.m_Length && strncmp(m_pData, other.m_pData, m_Length) == 0;
}

bool StringView::operator!=(const StringView& other) const
{
	return m_Length != other.m_Length || strncmp(m_pData, other.m_pData, m_Length) != 0;
}

bool StringView::operator==(const char* str) const
{
	return strncmp(m_pData, str, m_Length) == 0;
}

bool StringView::operator!=(const char* str) const
{
	return strncmp(m_pData, str, m_Length) != 0;
}

StringView::StringView(const char* str, u32 length)
	: m_pData(str)
	, m_Length(length)
{
	if (length == static_cast<u32>(-1) && str != nullptr)
	{
		m_Length = (u32)strlen(str);
	}
}

} // namespace ntt