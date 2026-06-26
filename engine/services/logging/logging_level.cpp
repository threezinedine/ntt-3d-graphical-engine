#include "logging_level.h"

namespace ntt {

const char* ToString(LoggingLevel level)
{
	switch (level)
	{
#define LOGGING_LEVEL_OPTION(option, value)                                                                            \
	case LOGGING_LEVEL_##option:                                                                                       \
		return #option;
#define LOGGING_LEVEL_OPTION_END(option, value)                                                                        \
	default:                                                                                                           \
		return "Unknown";
#include "logging_level.def"
#undef LOGGING_LEVEL_OPTION
#undef LOGGING_LEVEL_OPTION_END
	}

	return "Unknown";
}

ConsoleColor GetConsoleColor(LoggingLevel level)
{
	switch (level)
	{
#define LOGGING_LEVEL_OPTION(option, value)                                                                            \
	case LOGGING_LEVEL_##option:                                                                                       \
		return CONSOLE_COLOR_##value;
#define LOGGING_LEVEL_OPTION_END(option, value)                                                                        \
	default:                                                                                                           \
		return CONSOLE_COLOR_WHITE;
#include "logging_level.def"
#undef LOGGING_LEVEL_OPTION
#undef LOGGING_LEVEL_OPTION_END
	}

	return CONSOLE_COLOR_WHITE;
}

} // namespace ntt