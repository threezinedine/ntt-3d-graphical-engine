#pragma once

#include "core.h"
#include "logging_level.h"
#include "logging_type.h"

namespace ntt {

struct LoggingMessage
{
public:
	LoggingType	 type;
	LoggingLevel level;
	String		 file;
	u32			 line;
	String		 message;
	String		 finalMessage;

public:
	Result FormatMessage(const char* format);
	Result FormatMessage(StringView format);
};

} // namespace ntt
