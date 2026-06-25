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

} // namespace ntt