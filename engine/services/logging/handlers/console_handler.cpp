#include "console_handler.h"
#include "console.h"

namespace ntt {

ConsoleHandler::ConsoleHandler(const char* format, LoggingLevel level)
	: Handler(format, level)
{
}

ConsoleHandler::~ConsoleHandler()
{
}

Result ConsoleHandler::InitializeImpl()
{
	return RESULT_SUCCESS;
}

Result ConsoleHandler::HandleImpl(LoggingMessage& message)
{
	NTT_ASSERT_MSG(message.finalMessage.Length() > 0,
				   "Logging message is not formatted. Please format the message before handling it.");

	switch (message.level)
	{
#define LOGGING_LEVEL_OPTION(option, color)                                                                            \
	case LOGGING_LEVEL_##option:                                                                                       \
		setConsoleColor(CONSOLE_COLOR_##color);                                                                        \
		break;
#define LOGGING_LEVEL_OPTION_END(option, color)                                                                        \
	default:                                                                                                           \
		setConsoleColor(CONSOLE_COLOR_WHITE);                                                                          \
		break;
#include "../logging_level.def"
#undef LOGGING_LEVEL_OPTION
#undef LOGGING_LEVEL_OPTION_END
	}

	print("%s\n", message.finalMessage.CStr());

	return RESULT_SUCCESS;
}

Result ConsoleHandler::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
