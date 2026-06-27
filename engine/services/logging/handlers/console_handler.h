#pragma once

#include "handlers.h"

namespace ntt {

class ConsoleHandler : public Handler
{
public:
	ConsoleHandler(const char* format = NULL, LoggingLevel level = LOGGING_LEVEL_INFO);
	~ConsoleHandler();

protected:
	virtual Result InitializeImpl() override;
	virtual Result HandleImpl(LoggingMessage& message) override;
	virtual Result ShutdownImpl() override;
};

} // namespace ntt
