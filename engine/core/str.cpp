#include "str.h"
#include "string.h"
#include <string> // TODO: Remove later

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
		m_ReservedCapacity								   = 0;
	}
	else
	{
		m_IsShortString = false;
		m_pHeapBuffer	= ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)strlen(str) + 1).Cast<char>();
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		memcpy(m_pHeapBuffer.Get(), str, (u32)strlen(str) + 1);
		m_ReservedCapacity = (u32)strlen(str);
	}
}

StringView String::ToStringView() const
{
	return StringView(*this);
}

Result String::Reserve(u32 newCapacity)
{
	if (newCapacity <= m_ReservedCapacity)
	{
		return RESULT_NEW_CAPACITY_TOO_SMALL;
	}

	if (newCapacity <= NTT_SHORT_STRING_OPTIMIZATION_SIZE)
	{
		// No need to reserve, as the short string optimization can handle this
		return RESULT_SUCCESS;
	}

	Pointer<char> pNewHeapBuffer = ALLOCATOR_SAFE(m_pAllocator)->Allocate(newCapacity + 1).Cast<char>();
	memcpy(pNewHeapBuffer.Get(), CStr(), Length() + 1); // Copy existing string including null terminator

	if (m_pHeapBuffer != nullptr)
	{
		ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer.Get(), (u32)(strlen(m_pHeapBuffer.Get()) + 1));
	}
	m_pHeapBuffer	   = pNewHeapBuffer;
	m_IsShortString	   = false;
	m_ReservedCapacity = newCapacity;
	return RESULT_SUCCESS;
}

bool String::operator==(const String& other) const
{
	return this->Length() == other.Length() && strcmp(this->CStr(), other.CStr()) == 0;
}

bool String::operator!=(const String& other) const
{
	return !(*this == other);
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
		other.m_pHeapBuffer = nullptr; // Prevent double free
		m_ReservedCapacity	= other.m_ReservedCapacity;

		if (ALLOCATOR_SAFE(m_pAllocator) == ALLOCATOR_SAFE(other.m_pAllocator))
		{
			m_pHeapBuffer		= other.m_pHeapBuffer;
			other.m_pHeapBuffer = nullptr; // Prevent double free
		}
		else
		{
			// If allocators are different, we need to allocate new memory and copy the string
			m_pHeapBuffer = ALLOCATOR_SAFE(m_pAllocator)->Allocate(other.m_ReservedCapacity + 1).Cast<char>();
			memcpy(m_pHeapBuffer.Get(), other.m_pHeapBuffer.Get(), other.m_ReservedCapacity + 1);
			(ALLOCATOR_SAFE(other.m_pAllocator)->Free(other.m_pHeapBuffer.Get(), other.m_ReservedCapacity + 1));
			m_ReservedCapacity = other.m_ReservedCapacity;
		}
	}

	// TODO: transfer ownership of the allocator
	other.m_pAllocator	= nullptr; // Prevent double free of allocator if moved
	other.m_pHeapBuffer = nullptr; // Prevent double free of heap buffer if moved
}

String::~String()
{
	if (m_pHeapBuffer != nullptr)
	{
		NTT_ASSERT(m_pHeapBuffer.Free() == RESULT_SUCCESS);
		m_pHeapBuffer = nullptr; // Prevent dangling pointer
	}
}

const char* String::CStr() const
{
	return m_IsShortString ? m_pShortBuffer : m_pHeapBuffer.Get();
}

char* String::CStr()
{
	return m_IsShortString ? m_pShortBuffer : m_pHeapBuffer.Get();
}

u32 String::Length() const
{
	return m_IsShortString ? (u32)strlen(m_pShortBuffer) : (u32)strlen(m_pHeapBuffer.Get());
}

void String::operator=(String&& other) noexcept
{
	if (this != &other)
	{
		NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before moving

		// Free existing resources
		if (!m_IsShortString && m_pHeapBuffer != nullptr)
		{
			ALLOCATOR_SAFE(m_pAllocator)->Free(m_pHeapBuffer.Get(), m_ReservedCapacity + 1);
		}

		m_IsShortString = other.m_IsShortString;

		if (m_IsShortString)
		{
			memcpy(m_pShortBuffer, other.m_pShortBuffer, NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1);
			memset(other.m_pShortBuffer, 0, NTT_SHORT_STRING_OPTIMIZATION_SIZE + 1); // Clear the source buffer
		}
		else
		{
			if (ALLOCATOR_SAFE(m_pAllocator) == ALLOCATOR_SAFE(other.m_pAllocator))
			{
				m_pHeapBuffer		= other.m_pHeapBuffer;
				other.m_pHeapBuffer = nullptr; // Prevent double free
			}
			else
			{
				// If allocators are different, we need to allocate new memory and copy the string
				m_pHeapBuffer = ALLOCATOR_SAFE(m_pAllocator)->Allocate(other.m_ReservedCapacity + 1).Cast<char>();
				memcpy(m_pHeapBuffer.Get(), other.m_pHeapBuffer.Get(), other.m_ReservedCapacity + 1);
				(ALLOCATOR_SAFE(other.m_pAllocator)->Free(other.m_pHeapBuffer.Get(), other.m_ReservedCapacity + 1));
				m_ReservedCapacity = other.m_ReservedCapacity;
			}
		}
	}

	other.m_pAllocator	= nullptr; // Prevent double free of allocator if moved
	other.m_pHeapBuffer = nullptr; // Prevent double free of heap buffer if moved
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
		m_pHeapBuffer	= ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)strlen(str) + 1).Cast<char>();
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		memcpy(m_pHeapBuffer.Get(), str, (u32)strlen(str) + 1);
		m_ReservedCapacity = (u32)strlen(str);
	}
}

Result String::Clear()
{
	m_IsShortString = true;
	memset(m_pShortBuffer, 0, sizeof(m_pShortBuffer));
	if (m_pHeapBuffer != nullptr)
	{
		NTT_ASSERT_RESULT_SUCCESS(m_pHeapBuffer.Free());
		m_pHeapBuffer	   = nullptr;
		m_ReservedCapacity = 0;
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
		result.m_pHeapBuffer   = ALLOCATOR_SAFE(m_pAllocator)->Allocate(newLength + 1).Cast<char>();
		NTT_ASSERT_MSG(result.m_pHeapBuffer != nullptr, "Failed to allocate memory for concatenated string.");
		memcpy(result.m_pHeapBuffer.Get(), CStr(), (size_t)Length());
		memcpy(result.m_pHeapBuffer.Get() + (size_t)Length(), other.CStr(), (size_t)other.Length() + 1);
		result.m_ReservedCapacity = newLength;
		return result;
	}
}

void String::operator=(const StringView& str)
{
	NTT_ASSERT(Clear() == RESULT_SUCCESS); // Ensure the string is cleared before assignment

	if (str.Length() <= NTT_SHORT_STRING_OPTIMIZATION_SIZE)
	{
		m_IsShortString = true;
		m_pHeapBuffer	= nullptr;
		memcpy(m_pShortBuffer, str.Data(), str.Length());
		m_pShortBuffer[str.Length()] = '\0'; // Ensure null-termination
	}
	else
	{
		m_IsShortString = false;
		m_pHeapBuffer	= ALLOCATOR_SAFE(m_pAllocator)->Allocate(str.Length() + 1).Cast<char>();
		NTT_ASSERT_MSG(m_pHeapBuffer != nullptr, "Failed to allocate memory for heap string.");
		memcpy(m_pHeapBuffer.Get(), str.Data(), str.Length());
		m_pHeapBuffer.Get()[str.Length()] = '\0'; // Ensure null-termination
		m_ReservedCapacity				  = str.Length();
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
	return StringView(*this).Slice(start, length);
}

StringView StringView::Slice(u32 start, u32 length) const
{
	if (start >= Length())
	{
		return StringView(nullptr, 0); // Return an empty StringView if start is out of bounds
	}

	u32 end = start + length;
	if (end > Length() || length == u32(-1))
	{
		end = Length(); // Adjust end if it exceeds the string length
	}

	return StringView(m_pData + start, end - start);
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

String String::Join(const Array<String>& strings, const String& delimiter)
{
	if (strings.GetCount() == 0)
	{
		return String();
	}

	u32 totalLength = 0;
	for (const auto& str : strings)
	{
		totalLength += str.Length();
	}

	String result;
	result.Reserve(totalLength + (strings.GetCount() - 1) * delimiter.Length());

	char* pResultBuffer = result.m_IsShortString ? result.m_pShortBuffer : result.m_pHeapBuffer.Get();
	u32	  offset		= 0;

	for (u32 i = 0; i < strings.GetCount(); ++i)
	{
		const String& str = strings[i];
		memcpy(pResultBuffer + offset, str.CStr(), str.Length());
		offset += str.Length();

		if (i < strings.GetCount() - 1) // Don't add delimiter after the last string
		{
			memcpy(pResultBuffer + offset, delimiter.CStr(), delimiter.Length());
			offset += delimiter.Length();
		}
	}

	return result;
}

u32 StringView::Find(const StringView& subString) const
{
	if (Length() == 0)
	{
		return NTT_INVALID_INDEX;
	}

	if (subString.Length() == 0)
	{
		return 0;
	}

	if (subString.Length() > Length())
	{
		return NTT_INVALID_INDEX;
	}

	for (u32 i = 0; i <= Length() - subString.Length(); ++i)
	{
		if (strncmp(m_pData + i, subString.Data(), subString.Length()) == 0)
		{
			return i;
		}
	}

	return NTT_INVALID_INDEX; // Return invalid index if not found
}

u32 String::Find(const String& subString) const
{
	return StringView(*this).Find(StringView(subString));
}

Result String::Reset()
{
	NTT_ASSERT_RESULT_SUCCESS(ResetShort());
	NTT_ASSERT_RESULT_SUCCESS(ResetHeap());
	return RESULT_SUCCESS;
}

Result String::ResetShort()
{
	memset(m_pShortBuffer, 0, sizeof(m_pShortBuffer));
	return RESULT_SUCCESS;
}

Result String::ResetHeap()
{
	if (m_pHeapBuffer != nullptr)
	{
		if (m_IsShortString)
		{
			return RESULT_INVALID_STRING;
		}

		memset(m_pHeapBuffer.Get(), 0, strlen(m_pHeapBuffer.Get()) + 1);
	}
	return RESULT_SUCCESS;
}

#define DEFINE_PRIMITIVE_TO_STRING(type, function)                                                                     \
	template <>                                                                                                        \
	StringView ToString(const type& value)                                                                             \
	{                                                                                                                  \
		static char temp[18];                                                                                          \
		memset(temp, 0, sizeof(temp));                                                                                 \
		format(temp, sizeof(temp), function);                                                                          \
		return StringView(temp);                                                                                       \
	}                                                                                                                  \
	template <>                                                                                                        \
	StringView ToString(type& value)                                                                                   \
	{                                                                                                                  \
		static char temp[18];                                                                                          \
		memset(temp, 0, sizeof(temp));                                                                                 \
		format(temp, sizeof(temp), function);                                                                          \
		return StringView(temp);                                                                                       \
	}

template <>
StringView ToString(const String& value)
{
	return StringView(value.ToStringView());
}
template <>
StringView ToString(String& value)
{
	return StringView(value.ToStringView());
}

DEFINE_PRIMITIVE_TO_STRING(i32, std::to_string(value).c_str())
DEFINE_PRIMITIVE_TO_STRING(i16, std::to_string(value).c_str())
DEFINE_PRIMITIVE_TO_STRING(i8, std::to_string(value).c_str())

DEFINE_PRIMITIVE_TO_STRING(u32, std::to_string(value).c_str())
DEFINE_PRIMITIVE_TO_STRING(u16, std::to_string(value).c_str())
DEFINE_PRIMITIVE_TO_STRING(u8, std::to_string(value).c_str())

DEFINE_PRIMITIVE_TO_STRING(f64, std::to_string(value).c_str())
DEFINE_PRIMITIVE_TO_STRING(f32, std::to_string(value).c_str())

DEFINE_PRIMITIVE_TO_STRING(bool, value ? "true" : "false")

} // namespace ntt