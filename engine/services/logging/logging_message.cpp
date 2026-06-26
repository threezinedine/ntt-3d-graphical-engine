#include "logging_message.h"

namespace ntt {

Result LoggingMessage::FormatMessage(const char* format)
{
	return FormatMessage(StringView(format));
}

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
		finalMessageLength += StringView(ToString(this->attribute)).Length();                                          \
		messageIndex += StringView(keyword).Length();                                                                  \
		continue;                                                                                                      \
	}
#include "format_keyword.def"
#undef FORMAT_KEYWORD
		}

		finalMessageLength++;
		messageIndex++;
	}

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
		MemCopy(pFinalMessageBuffer + bufferIndex, attributeString.Data(), attributeString.Length());                  \
		bufferIndex += attributeString.Length();                                                                       \
		messageIndex += StringView(keyword).Length();                                                                  \
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
}

} // namespace ntt