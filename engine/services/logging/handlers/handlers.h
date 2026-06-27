#pragma once

#include "common.h"
#include "logging/logging_level.h"
#include "logging/logging_message.h"

namespace ntt {

class Handler
{
public:
	Handler(const char* format = NULL, LoggingLevel level = LOGGING_LEVEL_INFO);
	virtual ~Handler();

	Result Initialize();
	Result Handle(LoggingMessage& message);
	Result Shutdown();

public:
	inline LoggingLevel GetLevel() const
	{
		return m_Level;
	}

	inline void SetLevel(LoggingLevel level)
	{
		m_Level = level;
	}

protected:
	virtual Result InitializeImpl()					   = 0;
	virtual Result HandleImpl(LoggingMessage& message) = 0;
	virtual Result ShutdownImpl()					   = 0;

private:
	LoggingLevel m_Level;
	String		 m_Format;
};

} // namespace ntt

#include "console_handler.h"