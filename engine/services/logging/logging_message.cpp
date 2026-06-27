#include "logging_message.h"

namespace ntt {

Result LoggingMessage::FormatMessage(const char* format)
{
	return FormatMessage(StringView(format));
}

static bool StringToInteger(StringView str, i32* pResult);

static i32 AlignmentOffset(StringView next, i32* pAlignmentOffset)
{
	if (next.Length() == 0)
	{
		*pAlignmentOffset = 0;
		return 0;
	}

	if (next.Data()[0] != ':')
	{
		*pAlignmentOffset = 0;
		return 0;
	}

	u32 nextExclamationIndex = next.Find("!");

	if (nextExclamationIndex == NTT_INVALID_INDEX)
	{
		*pAlignmentOffset = 0;
		return 0;
	}

	for (u32 i = 1; i < nextExclamationIndex; ++i)
	{
		if (StringToInteger(next.Slice(1, nextExclamationIndex - 1), pAlignmentOffset))
		{
			return nextExclamationIndex + 1;
		}
	}

	*pAlignmentOffset = 0;
	return 0;
}

static bool StringToInteger(StringView str, i32* pResult)
{
	i32 result = 0;

	if (str.Data()[0] == '-')
	{
		for (u32 i = 1; i < str.Length(); ++i)
		{
			if (str.Data()[i] >= '0' && str.Data()[i] <= '9')
			{
				result = result * 10 + (str.Data()[i] - '0');
			}
			else
			{
				return false;
			}
		}

		result = -result;
	}
	else
	{
		for (u32 i = 0; i < str.Length(); ++i)
		{
			if (str.Data()[i] >= '0' && str.Data()[i] <= '9')
			{
				result = result * 10 + (str.Data()[i] - '0');
			}
			else
			{
				return false;
			}
		}
	}

	*pResult = result;
	return true;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

Result LoggingMessage::FormatMessage(StringView format)
{
	if (format.Length() == 0)
	{
		this->finalMessage.Clear();
		return RESULT_SUCCESS;
	}

	u32 finalMessageLength = 0;
	u32 messageIndex	   = 0;

	finalMessage.Reset();

	while (messageIndex < format.Length())
	{
		if (format[messageIndex] == '%')
		{
#define FORMAT_KEYWORD(keyword, attribute)                                                                             \
	if (format.Slice(messageIndex, StringView(keyword).Length()) == keyword)                                           \
	{                                                                                                                  \
		StringView attributeString = ToString(this->attribute);                                                        \
		i32		   alignmentOffset = 0;                                                                                \
		messageIndex += StringView(keyword).Length();                                                                  \
		messageIndex += AlignmentOffset(format.Slice(messageIndex), &alignmentOffset);                                 \
		if (ABS(alignmentOffset) > attributeString.Length())                                                           \
		{                                                                                                              \
			finalMessageLength += ABS(alignmentOffset);                                                                \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			finalMessageLength += attributeString.Length();                                                            \
		}                                                                                                              \
		continue;                                                                                                      \
	}
#include "format_keyword.def"
#undef FORMAT_KEYWORD
		}

		finalMessageLength++;
		messageIndex++;
	} // namespace ntt

	finalMessage.Reserve(finalMessageLength);

	messageIndex			  = 0;
	u32	  bufferIndex		  = 0;
	char* pFinalMessageBuffer = finalMessage.CStr();

	while (messageIndex < format.Length())
	{
		if (format[messageIndex] == '%')
		{
#define FORMAT_KEYWORD(keyword, attribute)                                                                             \
	if (format.Slice(messageIndex, StringView(keyword).Length()) == keyword)                                           \
	{                                                                                                                  \
		StringView attributeString = ToString(this->attribute);                                                        \
		i32		   alignmentOffset = 0;                                                                                \
		messageIndex += StringView(keyword).Length();                                                                  \
		messageIndex += AlignmentOffset(format.Slice(messageIndex), &alignmentOffset);                                 \
		i32 padding = ABS(alignmentOffset) - (i32)attributeString.Length();                                            \
		padding		= padding < 0 ? 0 : padding;                                                                       \
		i32 length	= (i32)attributeString.Length() + padding;                                                         \
		MemSet(pFinalMessageBuffer + bufferIndex, ' ', (u32)length);                                                   \
		if (alignmentOffset < 0)                                                                                       \
		{                                                                                                              \
			MemCopy(pFinalMessageBuffer + bufferIndex, attributeString.Data(), attributeString.Length());              \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			MemCopy(pFinalMessageBuffer + bufferIndex + padding, attributeString.Data(), attributeString.Length());    \
		}                                                                                                              \
		bufferIndex += length;                                                                                         \
		continue;                                                                                                      \
	}
#include "format_keyword.def"
#undef FORMAT_KEYWORD
		}

		MemCopy(pFinalMessageBuffer + bufferIndex, &format[messageIndex], 1);
		bufferIndex++;
		messageIndex++;
	}

	return RESULT_SUCCESS;
} // namespace ntt

} // namespace ntt