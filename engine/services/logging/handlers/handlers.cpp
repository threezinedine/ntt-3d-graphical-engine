#include "handlers.h"

namespace ntt {

Handler::Handler(const char* format, LoggingLevel level)
	: m_Level(level)
	, m_Format(format ? format : "")
{
}

Handler::~Handler()
{
}

Result Handler::Initialize()
{
	return InitializeImpl();
}

Result Handler::Handle(LoggingMessage& message)
{
	if (message.level < m_Level)
	{
		return RESULT_SUCCESS; // Skip handling messages below the handler's level
	}

	message.FormatMessage(m_Format.ToStringView());

	return HandleImpl(message);
}

Result Handler::Shutdown()
{
	return ShutdownImpl();
}

} // namespace ntt
