#include "logging.h"
#include "console.h"
#include "stdarg.h"
#include "stdio.h"

namespace ntt {

Array<Scope<Handler>> Logging::s_Handlers;

Result Logging::Initialize()
{
	s_Handlers.Append(MakeScope<ConsoleHandler>(g_GlobalAllocators.pMalloc));

	return RESULT_SUCCESS;
}

Result Logging::Log(LoggingType type, LoggingLevel level, const char* file, u32 line, const char* format, ...)
{
	char	buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	LoggingMessage message;
	message.type	= type;
	message.level	= level;
	message.file	= String(file);
	message.line	= line;
	message.message = String(buffer);

	for (const auto& handler : s_Handlers)
	{
		if (handler.Get() != nullptr)
		{
			handler.Get()->Handle(message);
		}
	}

	return RESULT_SUCCESS;
}

Result Logging::Shutdown()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
