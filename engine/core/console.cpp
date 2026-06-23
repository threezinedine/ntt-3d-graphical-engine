#include "console.h"
#include <cstdarg>
#include <cstdint>
#include <cstdio>

namespace ntt {

const char* ToString(ConsoleColor color)
{
	switch (color)
	{
#define CONSOLE_COLOR_OPTION(option, value)                                                                            \
	case CONSOLE_COLOR_##option:                                                                                       \
		return #option;
#define CONSOLE_COLOR_OPTION_END(option)                                                                               \
	default:                                                                                                           \
		return "Unknown ConsoleColor";
#include "console_color.def"
#undef CONSOLE_COLOR_OPTION
#undef CONSOLE_COLOR_OPTION_END
	}
}

void format(char* buffer, u32 bufferSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, bufferSize, format, args);
	va_end(args);
}

void print(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void setConsoleColor(ConsoleColor color)
{
	switch (color)
	{
#define CONSOLE_COLOR_OPTION(option, console_value)                                                                    \
	case CONSOLE_COLOR_##option:                                                                                       \
		printf("\033[%sm", console_value);                                                                             \
		break;
#define CONSOLE_COLOR_OPTION_END(option)
#include "console_color.def"
#undef CONSOLE_COLOR_OPTION
#undef CONSOLE_COLOR_OPTION_END
	}
}

void resetConsoleColor()
{
	printf("\033[0m");
}

} // namespace ntt
