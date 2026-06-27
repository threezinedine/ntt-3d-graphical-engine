#pragma once

#include "core.h"
#include "handlers/handlers.h"
#include "logging_level.h"
#include "logging_message.h"
#include "logging_type.h"
#include "smart_ptr/smart_ptr.h"

namespace ntt {

class Logging
{
public:
	static Result Initialize();
	static Result Log(LoggingType type, LoggingLevel level, const char* file, u32 line, const char* format, ...);
	static Result Shutdown();

private:
	static Array<Scope<Handler>> s_Handlers;
};

} // namespace ntt
