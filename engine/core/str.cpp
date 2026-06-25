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
		strncpy(m_pShortBuffer, str, NTT_SHORT_STRING_OPTIMIZATION_SIZE);
		m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE] = '\0'; // Ensure null-termination
	}
	else
	{
		m_IsShortString = false;
		m_pHeapBuffer	= (char*)ALLOCATOR_SAFE(m_pAllocator)->Allocate(strlen(str) + 1);
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		strcpy(m_pHeapBuffer, str);
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
		ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer, strlen(m_pHeapBuffer) + 1);
		m_pHeapBuffer = nullptr; // Prevent dangling pointer
	}
}

const char* String::CStr() const
{
	return m_IsShortString ? m_pShortBuffer : m_pHeapBuffer;
}

u32 String::Length() const
{
	return m_IsShortString ? strlen(m_pShortBuffer) : strlen(m_pHeapBuffer);
}

void String::operator=(String&& other) noexcept
{
	if (this != &other)
	{
		NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before moving

		// Free existing resources
		if (!m_IsShortString && m_pHeapBuffer != nullptr)
		{
			ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer, strlen(m_pHeapBuffer) + 1);
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
		strncpy(m_pShortBuffer, str, NTT_SHORT_STRING_OPTIMIZATION_SIZE);
		m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE] = '\0'; // Ensure null-termination
	}
	else
	{
		m_IsShortString = false;
		m_pHeapBuffer	= (char*)ALLOCATOR_SAFE(m_pAllocator)->Allocate(strlen(str) + 1);
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		strcpy(m_pHeapBuffer, str);
	}
}

Result String::Clear()
{
	m_IsShortString = true;
	memset(m_pShortBuffer, 0, sizeof(m_pShortBuffer));
	if (m_pHeapBuffer != nullptr)
	{
		NTT_ASSERT_RESULT_SUCCESS(ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer, strlen(m_pHeapBuffer) + 1));
		m_pHeapBuffer = nullptr;
	}
	return RESULT_SUCCESS;
}

String String::operator+(const String& other) const
{
	u32 newLength = Length() + other.Length();
	if (newLength <= NTT_SHORT_STRING_OPTIMIZATION_SIZE)
	{
		String result(m_pAllocator);
		result.m_IsShortString = true;
		strncpy(result.m_pShortBuffer, CStr(), NTT_SHORT_STRING_OPTIMIZATION_SIZE);
		strncat(
			result.m_pShortBuffer, other.CStr(), NTT_SHORT_STRING_OPTIMIZATION_SIZE - strlen(result.m_pShortBuffer));
		result.m_pShortBuffer[NTT_SHORT_STRING_OPTIMIZATION_SIZE] = '\0'; // Ensure null-termination
		return result;
	}
	else
	{
		String result(m_pAllocator);
		result.m_IsShortString = false;
		result.m_pHeapBuffer   = (char*)ALLOCATOR_SAFE(m_pAllocator)->Allocate(newLength + 1);
		NTT_ASSERT_MSG(result.m_pHeapBuffer != nullptr, "Failed to allocate memory for concatenated string.");
		strcpy(result.m_pHeapBuffer, CStr());
		strcat(result.m_pHeapBuffer, other.CStr());
		return result;
	}
}

} // namespace ntt