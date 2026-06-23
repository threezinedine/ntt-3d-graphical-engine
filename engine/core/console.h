#pragma once

#include "common.h"

namespace ntt {

enum ConsoleColor
{
#define CONSOLE_COLOR_OPTION(option, console_value) CONSOLE_COLOR_##option,
#define CONSOLE_COLOR_OPTION_END(option)			CONSOLE_COLOR_##option
#include "console_color.def"
#undef CONSOLE_COLOR_OPTION
#undef CONSOLE_COLOR_OPTION_END
};

const char* ToString(ConsoleColor color);

void setConsoleColor(ConsoleColor color);
void resetConsoleColor();
void format(char* buffer, u32 bufferSize, const char* format, ...);
void print(const char* format, ...);

} // namespace ntt
