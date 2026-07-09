#include "logging.h"
#include "console.h"
#include "stdarg.h"
#include "stdio.h"

namespace ntt {

Scope<Array<Scope<Handler>>> Logging::s_pHandlers;

Result Logging::Initialize()
{
	s_pHandlers = MakeScope<Array<Scope<Handler>>>(g_GlobalAllocators.pMalloc);

#if !NTT_UNITTEST
	s_pHandlers->Append(MakeScope<ConsoleHandler>(g_GlobalAllocators.pMalloc, "[%(type):-8!] %(message)"));
#endif // !NTT_UNITTEST

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

	for (const auto& handler : *(s_pHandlers.Get()))
	{
		if (handler != nullptr)
		{
			handler->Handle(message);
		}
	}

	return RESULT_SUCCESS;
}

Result Logging::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(s_pHandlers->Clear());
	s_pHandlers.Reset();
	return RESULT_SUCCESS;
}

Result Logging::SetLogLevel(LoggingLevel level)
{
	for (const auto& handler : *(s_pHandlers.Get()))
	{
		if (handler != nullptr)
		{
			handler->SetLevel(level);
		}
	}
	return RESULT_SUCCESS;
}

} // namespace ntt
