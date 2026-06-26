#pragma once

#include "core.h"

namespace ntt {

enum LoggingLevel
{
#define LOGGING_LEVEL_OPTION(option, value)		LOGGING_LEVEL_##option,
#define LOGGING_LEVEL_OPTION_END(option, value) LOGGING_LEVEL_##option
#include "logging_level.def"
#undef LOGGING_LEVEL_OPTION
#undef LOGGING_LEVEL_OPTION_END
};

const char*	 ToString(LoggingLevel level);
ConsoleColor GetConsoleColor(LoggingLevel level);

} // namespace ntt
