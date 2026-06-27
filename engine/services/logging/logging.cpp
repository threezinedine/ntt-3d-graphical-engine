#include "logging.h"
#include "console.h"
#include "stdarg.h"
#include "stdio.h"

namespace ntt {

Array<Handler*> Logging::s_Handlers;

Result Logging::Initialize()
{
	return RESULT_SUCCESS;
}

Result Logging::Log(LoggingType type, LoggingLevel level, const char* file, u32 line, const char* format, ...)
{
	char	buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	return RESULT_SUCCESS;
}

Result Logging::Shutdown()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
