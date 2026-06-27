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

	print("%s\n", message.finalMessage.CStr());

	return RESULT_SUCCESS;
}

Result ConsoleHandler::ShutdownImpl()
{
	return RESULT_SUCCESS;
}

} // namespace ntt
