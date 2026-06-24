#include "core.h"
#include <cstdio>

using namespace ntt;

int main(i32 argc, char** argv)
{
	g_Globals.argc = argc;
	g_Globals.argv = argv;

#define CONSOLE_COLOR_OPTION(option, console_value)                                                                    \
	setConsoleColor(CONSOLE_COLOR_##option);                                                                           \
	print("Line with %s!\n", ToString(CONSOLE_COLOR_##option));
#define CONSOLE_COLOR_OPTION_END(option)
#include "console_color.def"
#undef CONSOLE_COLOR_OPTION
#undef CONSOLE_COLOR_OPTION_END
	resetConsoleColor();
	print("Line with reset color!\n");

	return 0;
}